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

	Phase3ChaseSpeed = 800.0f;
	Phase3StopDistance = 100.0f;
	PlayerLookCheckAngle = 15.0f;
	Phase3HorrorMesh = nullptr;
    
	bIsInPhase3Mode = false;
	bIsPlayerLookingAtMe = false;
	OriginalMesh = nullptr;
	Phase3HorrorScale = FVector(1.0f, 1.0f, 1.0f);

	SweepAttackDamage = 30.0f;
	SweepAttackRange = 500.0f;
	SweepAttackCooldown = 5.0f;
	bCanSweepAttack = true;
	SweepAttackEffect = nullptr;
	bShowSweepDebug = true;

	SweepAttackHitCount = 5;
	SweepAttackHitInterval = 0.1f;
	CurrentHitCount = 0;
}

void ASLMouseActor::BeginPlay()
{
	Super::BeginPlay();

	// 메시 컴포넌트 콜리전 설정
	if (MouseMeshComponent)
	{
		MouseMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MouseMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	// 콜리전 컴포넌트 설정
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &ASLMouseActor::OnCollisionHit);
		CollisionComponent->SetSphereRadius(DetectionRange);
		
		// 플레이어와는 겹침만 허용, 물리적 충돌 방지
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
		CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
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

}

EMouseActorState ASLMouseActor::GetCurrentState() const
{
	return CurrentState;
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
		if (bIsInPhase3Mode)
		{
			// Phase3에서는 겹침만 허용
			CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
			CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
		}
		else
		{
			// 일반 모드에서는 기존 설정
			CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
			CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
			CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		}
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
		LookAtRotation.Yaw -= 90.0f; 
		LookAtRotation.Pitch -= 20.0f; 
		LookAtRotation.Roll -= 10.0f; 
		
		
		FRotator CurrentRotation = MouseMeshComponent->GetComponentRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 5.0f);
		MouseMeshComponent->SetWorldRotation(NewRotation);
		
		
		FVector DirectionToPlayer = (PlayerLocation - GetActorLocation()).GetSafeNormal();
		FRotator ActorRotation = DirectionToPlayer.Rotation();
		FRotator CurrentActorRotation = GetActorRotation();
		FRotator NewActorRotation = FMath::RInterpTo(CurrentActorRotation, ActorRotation, DeltaTime, 3.0f);
		SetActorRotation(NewActorRotation);
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
	if (bIsInPhase3Mode)
	{
		return false;
	}
    
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
	SetMouseActorState(EMouseActorState::Descending);
	
	// Phase3에서는 콜리전을 더 작게 설정하고 겹침만 허용
	if (CollisionComponent)
	{
		CollisionComponent->SetSphereRadius(DetectionRange * 0.5f); // 콜리전 범위 축소
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
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
   
	// 스윕 공격 범위 내에 있고 쿨타임이 끝났으면 공격
	if (DistanceToPlayer <= SweepAttackRange && bCanSweepAttack)
	{
		PerformSweepAttack();
		return;
	}
   
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

	// 각도 확인
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

void ASLMouseActor::PerformSweepAttack()
{
	if (!bCanSweepAttack || !bIsInPhase3Mode || !IsValid(TargetPlayer))
	{
		return;
	}
    
	bCanSweepAttack = false;
	CurrentHitCount = 0;
	MultiHitTargets.Empty();
    
	// 나이아가라 이펙트 재생 (플레이어 방향으로)
	if (IsValid(SweepAttackEffect) && IsValid(GetWorld()))
	{
		FVector StartLocation = GetActorLocation();
		FVector PlayerLocation = TargetPlayer->GetActorLocation();
		
		// 플레이어 방향 계산 (수평면에서)
		FVector DirectionToPlayer = (PlayerLocation - StartLocation);
		DirectionToPlayer.Z = 0.0f; // 수평 방향만 고려
		DirectionToPlayer.Normalize();
		
		// 이펙트 회전 계산 (Up Vector가 플레이어를 향하도록)
		FRotator EffectRotation = UKismetMathLibrary::MakeRotFromZ(DirectionToPlayer);
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			SweepAttackEffect,
			StartLocation,
			EffectRotation
		);
		
		UE_LOG(LogTemp, Warning, TEXT("Mouse Actor: Effect spawned at rotation Yaw=%f, Player direction=%s"), 
			EffectRotation.Yaw, *DirectionToPlayer.ToString());
	}
    
	// 첫 번째 히트 실행
	ExecuteSweepAttack();
	CurrentHitCount++;
    
	// 다단 히트 타이머 시작
	if (SweepAttackHitCount > 1)
	{
		StartMultiHitTimer();
	}
    
	// 쿨타임 시작
	if (IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().SetTimer(
			SweepAttackCooldownTimer,
			this,
			&ASLMouseActor::OnSweepAttackCooldownFinished,
			SweepAttackCooldown,
			false
		);
	}
}

void ASLMouseActor::ExecuteSweepAttack()
{
    if (!IsValid(GetWorld()) || !IsValid(TargetPlayer))
    {
        return;
    }
    
    FVector StartLocation = GetActorLocation();
    
    // 플레이어 방향으로 스윕 공격
    FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - StartLocation).GetSafeNormal();
    FVector EndLocation = StartLocation + (DirectionToPlayer * SweepAttackRange);

    if (bShowSweepDebug)
    {
        // 디버그 박스를 플레이어 방향으로 표시
        FVector CenterPoint = (StartLocation + EndLocation) * 0.5f;
        FRotator BoxRotation = DirectionToPlayer.Rotation();
        
        FColor DebugColor = (CurrentHitCount == 1) ? FColor::Red : FColor::Orange;
        
        DrawDebugBox(
            GetWorld(),
            CenterPoint,
            FVector(SweepAttackRange * 0.5f, 50.0f, 50.0f),
            BoxRotation.Quaternion(),
            DebugColor,
            false,
            SweepAttackHitInterval + 0.5f
        );
    }
    
    // 스윕 파라미터 설정
    FCollisionShape SweepShape = FCollisionShape::MakeBox(FVector(50.0f, 50.0f, 50.0f));
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    
    TArray<FHitResult> HitResults;
    
    // 플레이어 방향으로 스윕 실행
    FQuat SweepRotation = DirectionToPlayer.Rotation().Quaternion();
    
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        StartLocation,
        EndLocation,
        SweepRotation,
        ECC_Pawn,
        SweepShape,
        QueryParams
    );
    
    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            if (AActor* HitActor = Hit.GetActor())
            {
                // 첫 번째 히트이거나 다단 히트에서 이미 맞은 대상인 경우 처리
                if (CurrentHitCount == 1 || MultiHitTargets.Contains(HitActor))
                {
                    // 첫 번째 히트일 때 다단 히트 대상에 추가
                    if (CurrentHitCount == 1)
                    {
                        MultiHitTargets.Add(HitActor);
                    }
                    
                    // 배틀 컴포넌트를 통해 데미지 적용
                    if (UBattleComponent* TargetBattleComp = HitActor->FindComponentByClass<UBattleComponent>())
                    {
                        float CurrentDamage = SweepAttackDamage;
                        
                        // 다단 히트일 경우 데미지 조정 (선택사항)
                        if (CurrentHitCount > 1)
                        {
                            CurrentDamage *= 0.7f; // 후속 히트는 70% 데미지
                        }
                        
                        TargetBattleComp->ReceiveHitResult(
                            CurrentDamage,
                            this,
                            Hit,
                            EAttackAnimType::AAT_AIProjectile
                        );
                        
                        UE_LOG(LogTemp, Warning, TEXT("Mouse Actor Phase3: Sweep attack hit %s (Hit %d/%d)"), 
                            *HitActor->GetName(), CurrentHitCount, SweepAttackHitCount);
                    }
                }
            }
        }
    }
}

void ASLMouseActor::OnSweepAttackCooldownFinished()
{
    bCanSweepAttack = true;
}

void ASLMouseActor::StartMultiHitTimer()
{
	if (IsValid(GetWorld()) && CurrentHitCount < SweepAttackHitCount)
	{
		GetWorld()->GetTimerManager().SetTimer(
			MultiHitTimer,
			this,
			&ASLMouseActor::OnMultiHitTimerFinished,
			SweepAttackHitInterval,
			false
		);
	}
}

void ASLMouseActor::OnMultiHitTimerFinished()
{
	if (CurrentHitCount < SweepAttackHitCount)
	{
		CurrentHitCount++;
		ExecuteSweepAttack();
        
		// 다음 히트가 있으면 타이머 재시작
		if (CurrentHitCount < SweepAttackHitCount)
		{
			StartMultiHitTimer();
		}
		else
		{
			// 모든 히트 완료
			UE_LOG(LogTemp, Warning, TEXT("Mouse Actor: Multi-hit sweep attack completed (%d hits)"), SweepAttackHitCount);
		}
	}
}
