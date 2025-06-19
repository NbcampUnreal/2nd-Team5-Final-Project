#include "SLMouseActor.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/SLPlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

ASLMouseActor::ASLMouseActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	MouseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MouseMeshComponent"));
	MouseMeshComponent->SetupAttachment(RootComponent);
	MouseMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));

	OrbitRadius = 300.0f;
	OrbitHeight = 400.0f;
	OrbitSpeed = 90.0f;
	MoveToOrbitSpeed = 500.0f;
	DescentSpeed = 800.0f;
	GrabMoveSpeed = 600.0f;
	GrabDistance = 500.0f;
	GrabHeight = 300.0f;
	GrabDamage = 20.0f;
	GrabCooldownMin = 2.0f;
	GrabCooldownMax = 5.0f;
	CollisionDisableTime = 2.0f;
	MaxHealth = 100.0f;
	DetectionRange = 100.0f;

	CurrentState = EMouseActorState::Inactive;
	CurrentHealth = 0.0f;
	TargetPlayer = nullptr;
	GrabTargetLocation = FVector::ZeroVector;
	MoveToOrbitTargetLocation = FVector::ZeroVector;
	bCanGrab = true;
	OrbitAngle = 0.0f;
	OrbitCenter = FVector::ZeroVector;

	GrabEffect = nullptr;
	DestroyEffect = nullptr;
	GrabSound = nullptr;
	DestroySound = nullptr;

	Phase3ChaseSpeed = 1200.0f;
	Phase3StopDistance = 200.0f;
	PlayerLookCheckAngle = 30.0f;
	Phase3HorrorMesh = nullptr;
	Phase3HorrorMaterial = nullptr;
    
	bIsInPhase3Mode = false;
	bIsPlayerLookingAtMe = false;
	OriginalMesh = nullptr;
	Phase3HorrorScale = FVector(2.0f, 2.0f, 2.0f);


}

void ASLMouseActor::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &ASLMouseActor::OnCollisionHit);
		CollisionComponent->SetSphereRadius(DetectionRange);
	}

	if (BattleComponent)
	{
		BattleComponent->OnCharacterHited.AddDynamic(this, &ASLMouseActor::OnBattleComponentHit);
	}

	TargetPlayer = FindPlayerCharacter();
	CurrentHealth = MaxHealth;
	bCanGrab = true;
	OrbitAngle = 0.0f;
}

void ASLMouseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	if (!IsValid(TargetPlayer))
	{
		TargetPlayer = FindPlayerCharacter();
		if (!IsValid(TargetPlayer))
		{
			return;
		}
	}

	if (bIsInPhase3Mode)
	{
		UpdatePhase3Movement(DeltaTime);
		UpdateMeshRotation(DeltaTime);
		return;
	}

	switch (CurrentState)
	{
	case EMouseActorState::MovingToOrbit:
		UpdateMoveToOrbitMovement(DeltaTime);
		break;
        
	case EMouseActorState::Orbiting:
		UpdateOrbitMovement(DeltaTime);
		if (CanGrabPlayer())
		{
			SetMouseActorState(EMouseActorState::Descending);
		}
		break;
        
	case EMouseActorState::Descending:
		UpdateDescentMovement(DeltaTime);
		if (IsPlayerInRange())
		{
			StartGrabPlayer();
		}
		break;
        
	case EMouseActorState::Grabbing:
		UpdateGrabMovement(DeltaTime);
		break;
	}

	UpdateMeshRotation(DeltaTime);
}

void ASLMouseActor::StartOrbiting()
{
	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	TargetPlayer = FindPlayerCharacter();
	
	if (!IsValid(TargetPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Mouse Actor: No valid player found for orbiting"));
		return;
	}

	OrbitCenter = TargetPlayer->GetActorLocation();
	OrbitAngle = 0.0f;
	
	MoveToOrbitTargetLocation = CalculateOrbitPosition();
	
	SetMouseActorState(EMouseActorState::MovingToOrbit);
	
	UE_LOG(LogTemp, Log, TEXT("Mouse Actor moving to orbit position from current location"));
}

void ASLMouseActor::StopOrbiting()
{
	if (CurrentState == EMouseActorState::Grabbing && IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetPlayer->CharacterDragged(false);
		
		if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
		{
			MovementComp->SetDefaultMovementMode();
		}
	}

	SetMouseActorState(EMouseActorState::Inactive);
	
	if (IsValid(CollisionComponent))
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor stopped orbiting"));
}

void ASLMouseActor::DestroyMouseActor()
{
	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	if (CurrentState == EMouseActorState::Grabbing && IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetPlayer->CharacterDragged(false);
		
		if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
		{
			MovementComp->SetDefaultMovementMode();
		}
	}

	SetMouseActorState(EMouseActorState::Destroyed);

	if (IsValid(DestroyEffect) && IsValid(GetWorld()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (IsValid(DestroySound) && IsValid(GetWorld()))
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			DestroySound,
			GetActorLocation()
		);
	}

	if (OnMouseActorDestroyed.IsBound())
	{
		OnMouseActorDestroyed.Broadcast(this);
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor destroyed"));
}

EMouseActorState ASLMouseActor::GetCurrentState() const
{
	return CurrentState;
}

void ASLMouseActor::SetOrbitSettings(float NewOrbitRadius, float NewOrbitHeight, float NewOrbitSpeed)
{
	OrbitRadius = FMath::Max(0.0f, NewOrbitRadius);
	OrbitHeight = FMath::Max(0.0f, NewOrbitHeight);
	OrbitSpeed = FMath::Max(0.0f, NewOrbitSpeed);
}

void ASLMouseActor::SetGrabSettings(float NewGrabDistance, float NewGrabHeight, float NewGrabDamage, float NewGrabCooldownMin, float NewGrabCooldownMax)
{
	GrabDistance = FMath::Max(0.0f, NewGrabDistance);
	GrabHeight = FMath::Max(0.0f, NewGrabHeight);
	GrabDamage = FMath::Max(0.0f, NewGrabDamage);
	GrabCooldownMin = FMath::Max(0.1f, NewGrabCooldownMin);
	GrabCooldownMax = FMath::Max(GrabCooldownMin, NewGrabCooldownMax);
}

void ASLMouseActor::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor) || CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	if (ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(OtherActor))
	{
		if (IsValid(PlayerCharacter) && CurrentState == EMouseActorState::Descending && CanGrabPlayer())
		{
			StartGrabPlayer();
		}
	}
}

void ASLMouseActor::OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType)
{
	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	CurrentHealth -= DamageAmount;

	if (CurrentHealth <= 0.0f)
	{
		DestroyMouseActor();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Mouse Actor damaged: %f, Health remaining: %f"), 
			DamageAmount, CurrentHealth);
	}
}

void ASLMouseActor::OnGrabCooldownFinished()
{
	bCanGrab = true;
	OnMouseGrabCooldownFinished.Broadcast();
}

void ASLMouseActor::OnCollisionReenabled()
{
	if (IsValid(CollisionComponent))
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	OnMouseCollisionReenabled.Broadcast();
}

void ASLMouseActor::UpdateOrbitMovement(float DeltaTime)
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	OrbitCenter = TargetPlayer->GetActorLocation();
	
	OrbitAngle += OrbitSpeed * DeltaTime;
	if (OrbitAngle >= 360.0f)
	{
		OrbitAngle -= 360.0f;
	}

	FVector OrbitPosition = CalculateOrbitPosition();
	SetActorLocation(OrbitPosition);
}

void ASLMouseActor::UpdateDescentMovement(float DeltaTime)
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	FVector PlayerLocation = TargetPlayer->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	FVector DirectionToPlayer = (PlayerLocation - CurrentLocation).GetSafeNormal();
	
	if (DirectionToPlayer.IsZero())
	{
		return;
	}

	FVector MovementDelta = DirectionToPlayer * DescentSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + MovementDelta;
	SetActorLocation(NewLocation);
}

void ASLMouseActor::UpdateGrabMovement(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector DirectionToTarget = (GrabTargetLocation - CurrentLocation).GetSafeNormal();
	
	if (DirectionToTarget.IsZero())
	{
		return;
	}

	FVector MovementDelta = DirectionToTarget * GrabMoveSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + MovementDelta;
	SetActorLocation(NewLocation);

	float DistanceToTarget = FVector::Dist(CurrentLocation, GrabTargetLocation);
	if (DistanceToTarget <= 50.0f)
	{
		CompleteGrabPlayer();
	}
}

void ASLMouseActor::UpdateMeshRotation(float DeltaTime)
{
	if (!IsValid(MouseMeshComponent) || !IsValid(TargetPlayer))
	{
		return;
	}

	FVector MeshLocation = MouseMeshComponent->GetComponentLocation();
	FVector PlayerLocation = TargetPlayer->GetActorLocation();
    
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MeshLocation, PlayerLocation);
    
	if (bIsInPhase3Mode)
	{
		// Phase3에서는 즉시 플레이어를 향해 회전 (더 섬뜩함)
		LookAtRotation.Yaw -= 90.0f; // 정면 맞춤
		LookAtRotation.Pitch -= 10.0f; // 살짝 아래를 내려다보는 느낌
        
		// 즉시 회전
		MouseMeshComponent->SetWorldRotation(LookAtRotation);
	}
	else
	{
		// 기존 부드러운 회전
		LookAtRotation.Yaw -= 90.0f;
		LookAtRotation.Pitch -= 45.0f;
        
		FRotator CurrentRotation = MouseMeshComponent->GetComponentRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 3.0f);
		MouseMeshComponent->SetWorldRotation(NewRotation);
	}
}

void ASLMouseActor::UpdateMoveToOrbitMovement(float DeltaTime)
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	OrbitCenter = TargetPlayer->GetActorLocation();
	MoveToOrbitTargetLocation = CalculateOrbitPosition();

	FVector CurrentLocation = GetActorLocation();
	FVector DirectionToTarget = (MoveToOrbitTargetLocation - CurrentLocation).GetSafeNormal();
	
	if (DirectionToTarget.IsZero())
	{
		SetMouseActorState(EMouseActorState::Orbiting);
		return;
	}

	FVector MovementDelta = DirectionToTarget * MoveToOrbitSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + MovementDelta;
	SetActorLocation(NewLocation);

	float DistanceToTarget = FVector::Dist(CurrentLocation, MoveToOrbitTargetLocation);
	if (DistanceToTarget <= 50.0f)
	{
		SetMouseActorState(EMouseActorState::Orbiting);
		UE_LOG(LogTemp, Log, TEXT("Mouse Actor reached orbit position, starting to orbit"));
	}
}

void ASLMouseActor::StartGrabPlayer()
{
	if (!IsValid(TargetPlayer) || CurrentState != EMouseActorState::Descending || !CanGrabPlayer())
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Grabbing);
	bCanGrab = false;

	FVector CurrentLocation = GetActorLocation();
	FVector DirectionFromPlayer = (CurrentLocation - TargetPlayer->GetActorLocation()).GetSafeNormal();
	GrabTargetLocation = CurrentLocation + (DirectionFromPlayer * GrabDistance);
	GrabTargetLocation.Z += GrabHeight;

	TargetPlayer->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	TargetPlayer->CharacterDragged(true);
	
	if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
	{
		MovementComp->DisableMovement();
	}

	if (IsValid(GrabEffect) && IsValid(GetWorld()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			GrabEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (IsValid(GrabSound) && IsValid(GetWorld()))
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			GrabSound,
			GetActorLocation()
		);
	}
}

void ASLMouseActor::CompleteGrabPlayer()
{
	if (CurrentState != EMouseActorState::Grabbing)
	{
		return;
	}

	if (IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetPlayer->CharacterDragged(false);
		if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
		{
			MovementComp->SetDefaultMovementMode();
		}
	}

	if (IsValid(CollisionComponent))
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		// 델리게이트로 변경 - 타이머 대신 지연 실행
		FTimerHandle CollisionTimer;
		GetWorldTimerManager().SetTimer(
			CollisionTimer,
			this,
			&ASLMouseActor::OnCollisionReenabled,
			CollisionDisableTime,
			false
		);
	}

	ApplyGrabDamage();

	// 델리게이트로 변경 - 타이머 대신 지연 실행
	float RandomCooldown = GetRandomGrabCooldown();
	FTimerHandle CooldownTimer;
	GetWorldTimerManager().SetTimer(
		CooldownTimer,
		this,
		&ASLMouseActor::OnGrabCooldownFinished,
		RandomCooldown,
		false
	);
	
	SetMouseActorState(EMouseActorState::Orbiting);
	
	if (IsValid(TargetPlayer))
	{
		OrbitCenter = TargetPlayer->GetActorLocation();
		OrbitAngle = 0.0f;
		
		FVector OrbitPosition = CalculateOrbitPosition();
		SetActorLocation(OrbitPosition);
	}

	OnMouseGrabCompleted.Broadcast();
}

void ASLMouseActor::ApplyGrabDamage()
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	if (UBattleComponent* PlayerBattleComp = TargetPlayer->FindComponentByClass<UBattleComponent>())
	{
		FHitResult HitResult;
		HitResult.ImpactPoint = TargetPlayer->GetActorLocation();
		HitResult.Location = TargetPlayer->GetActorLocation();
		HitResult.ImpactNormal = (GetActorLocation() - TargetPlayer->GetActorLocation()).GetSafeNormal();
		HitResult.Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
		HitResult.TraceStart = GetActorLocation();
		HitResult.TraceEnd = TargetPlayer->GetActorLocation();
		
		PlayerBattleComp->ReceiveHitResult(GrabDamage, this, HitResult, EAttackAnimType::AAT_AISpecial);
	}
}

void ASLMouseActor::SetMouseActorState(EMouseActorState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		OnMouseStateChanged.Broadcast(NewState);
	}
}

ASLPlayerCharacter* ASLMouseActor::FindPlayerCharacter()
{
	if (!IsValid(GetWorld()))
	{
		return nullptr;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return nullptr;
	}

	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!IsValid(PlayerPawn))
	{
		return nullptr;
	}

	return Cast<ASLPlayerCharacter>(PlayerPawn);
}

bool ASLMouseActor::IsPlayerInRange() const
{
	if (!IsValid(TargetPlayer))
	{
		return false;
	}

	float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
	return DistanceToPlayer <= DetectionRange;
}

bool ASLMouseActor::CanGrabPlayer() const
{
	return bCanGrab && (CurrentState == EMouseActorState::Orbiting || CurrentState == EMouseActorState::Descending);
}

float ASLMouseActor::GetRandomGrabCooldown() const
{
   return FMath::FRandRange(GrabCooldownMin, GrabCooldownMax);
}

FVector ASLMouseActor::CalculateOrbitPosition() const
{
   float RadianAngle = FMath::DegreesToRadians(OrbitAngle);
   FVector OrbitPosition = OrbitCenter;
   OrbitPosition.X += FMath::Cos(RadianAngle) * OrbitRadius;
   OrbitPosition.Y += FMath::Sin(RadianAngle) * OrbitRadius;
   OrbitPosition.Z += OrbitHeight;
   
   return OrbitPosition;
}

void ASLMouseActor::StartPhase3HorrorMode()
{
   if (CurrentState == EMouseActorState::Destroyed)
   {
       return;
   }

   bIsInPhase3Mode = true;
   ChangeToHorrorAppearance();
   SetMouseActorState(EMouseActorState::Descending);
   
   UE_LOG(LogTemp, Display, TEXT("Mouse Actor: Phase 3 Horror Mode Started"));
}

void ASLMouseActor::StopPhase3HorrorMode()
{
   bIsInPhase3Mode = false;
   RestoreOriginalAppearance();
   
   UE_LOG(LogTemp, Display, TEXT("Mouse Actor: Phase 3 Horror Mode Stopped"));
}

void ASLMouseActor::SetPhase3Settings(float NewChaseSpeed, float NewStopDistance, float NewLookAngle)
{
   Phase3ChaseSpeed = FMath::Max(0.0f, NewChaseSpeed);
   Phase3StopDistance = FMath::Max(0.0f, NewStopDistance);
   PlayerLookCheckAngle = FMath::Clamp(NewLookAngle, 0.0f, 180.0f);
}

void ASLMouseActor::SetPhase3Scale(const FVector& NewScale)
{
	Phase3HorrorScale = NewScale;
    
	// 이미 Phase3 모드라면 즉시 적용
	if (bIsInPhase3Mode && IsValid(MouseMeshComponent))
	{
		MouseMeshComponent->SetWorldScale3D(Phase3HorrorScale);
	}
}

void ASLMouseActor::UpdatePhase3Movement(float DeltaTime)
{
   if (!IsValid(TargetPlayer))
   {
       return;
   }

   bIsPlayerLookingAtMe = IsPlayerLookingAtMe();
   
   if (bIsPlayerLookingAtMe)
   {
       return;
   }

   FVector PlayerLocation = TargetPlayer->GetActorLocation();
   FVector CurrentLocation = GetActorLocation();
   float DistanceToPlayer = FVector::Dist(CurrentLocation, PlayerLocation);
   
   if (DistanceToPlayer > Phase3StopDistance)
   {
       FVector DirectionToPlayer = (PlayerLocation - CurrentLocation).GetSafeNormal();
       FVector MovementDelta = DirectionToPlayer * Phase3ChaseSpeed * DeltaTime;
       FVector NewLocation = CurrentLocation + MovementDelta;
       SetActorLocation(NewLocation);
   }
}

bool ASLMouseActor::IsPlayerLookingAtMe() const
{
	if (!IsValid(TargetPlayer))
	{
		return false;
	}

	// 플레이어 컨트롤러에서 카메라 정보 가져오기
	APlayerController* PlayerController = Cast<APlayerController>(TargetPlayer->GetController());
	if (!PlayerController)
	{
		return false;
	}

	// 카메라 위치와 방향 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector CameraForward = CameraRotation.Vector();
	FVector MouseLocation = GetActorLocation();

	// 카메라에서 마우스 액터로의 방향
	FVector DirectionToMouse = (MouseLocation - CameraLocation).GetSafeNormal();

	// 각도 확인 (시야각 체크)
	float DotProduct = FVector::DotProduct(CameraForward, DirectionToMouse);
	DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);
	float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

	// 시야각 범위 밖이면 false
	if (AngleInDegrees > PlayerLookCheckAngle)
	{
		return false;
	}

	// 레이캐스트로 실제로 보이는지 확인
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(TargetPlayer);
	QueryParams.bTraceComplex = false;

	// 카메라에서 마우스 액터로 레이캐스트
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		MouseLocation,
		ECC_Visibility,
		QueryParams
	);

	return !bHit;
}

void ASLMouseActor::ChangeToHorrorAppearance()
{
   if (!IsValid(MouseMeshComponent))
   {
       return;
   }

   OriginalMesh = MouseMeshComponent->GetStaticMesh();

   if (IsValid(Phase3HorrorMesh))
   {
       MouseMeshComponent->SetStaticMesh(Phase3HorrorMesh);
   }

	MouseMeshComponent->SetWorldScale3D(Phase3HorrorScale);
}

void ASLMouseActor::RestoreOriginalAppearance()
{
   if (!IsValid(MouseMeshComponent))
   {
       return;
   }

   if (IsValid(OriginalMesh))
   {
       MouseMeshComponent->SetStaticMesh(OriginalMesh);
   }
	
	MouseMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
}