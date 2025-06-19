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

	// Components Setup
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

	// Initialize values
	OrbitRadius = 300.0f;
	OrbitHeight = 400.0f;
	OrbitSpeed = 90.0f;
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

	// Runtime values
	CurrentState = EMouseActorState::Inactive;
	CurrentHealth = MaxHealth;
	TargetPlayer = nullptr;
	GrabTargetLocation = FVector::ZeroVector;
	bCanGrab = true;
	OrbitAngle = 0.0f;
	OrbitCenter = FVector::ZeroVector;

	// Effects
	GrabEffect = nullptr;
	DestroyEffect = nullptr;
	GrabSound = nullptr;
	DestroySound = nullptr;
}

void ASLMouseActor::BeginPlay()
{
	Super::BeginPlay();

	// Setup collision events
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &ASLMouseActor::OnCollisionHit);
		CollisionComponent->SetSphereRadius(DetectionRange);
	}

	// Setup battle component
	if (BattleComponent)
	{
		BattleComponent->OnCharacterHited.AddDynamic(this, &ASLMouseActor::OnBattleComponentHit);
	}

	// Find initial target
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

	// Find player if we don't have one
	if (!IsValid(TargetPlayer))
	{
		TargetPlayer = FindPlayerCharacter();
		if (!IsValid(TargetPlayer))
		{
			return;
		}
	}

	// Update behavior based on current state
	switch (CurrentState)
	{
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

	// Update mesh rotation to look at player
	UpdateMeshRotation(DeltaTime);
}

void ASLMouseActor::StartOrbiting()
{
	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Orbiting);
	TargetPlayer = FindPlayerCharacter();
	
	if (IsValid(TargetPlayer))
	{
		// Set initial orbit center and position
		OrbitCenter = TargetPlayer->GetActorLocation();
		OrbitAngle = 0.0f;
		
		// Position mouse actor above and to the side of player
		FVector InitialOrbitPosition = OrbitCenter;
		InitialOrbitPosition.X += OrbitRadius;
		InitialOrbitPosition.Z += OrbitHeight;
		SetActorLocation(InitialOrbitPosition);
	}
	
	UE_LOG(LogTemp, Log, TEXT("Mouse Actor started orbiting above player"));
}

void ASLMouseActor::StopOrbiting()
{
	// Release player if currently grabbing
	if (CurrentState == EMouseActorState::Grabbing && IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetPlayer->CharacterDragged(false);
		
		// Re-enable player movement
		if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
		{
			MovementComp->SetDefaultMovementMode();
		}
	}

	SetMouseActorState(EMouseActorState::Inactive);
	
	// Clear timers
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().ClearTimer(GrabCooldownTimerHandle);
		GetWorldTimerManager().ClearTimer(CollisionTimerHandle);
	}

	// Re-enable collision if it was disabled
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

	// 현재 잡고 있는 경우 플레이어 해제
	if (CurrentState == EMouseActorState::Grabbing && IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetPlayer->CharacterDragged(false);
		
		// 플레이어 이동 다시 활성화
		if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
		{
			MovementComp->SetDefaultMovementMode();
		}
	}

	SetMouseActorState(EMouseActorState::Destroyed);

	// 파괴 효과 재생
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

	// Clear timers
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().ClearTimer(GrabCooldownTimerHandle);
		GetWorldTimerManager().ClearTimer(CollisionTimerHandle);
	}

	// Broadcast destruction
	if (OnMouseActorDestroyed.IsBound())
	{
		OnMouseActorDestroyed.Broadcast(this);
	}

	// Hide and disable
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor destroyed"));
}

EMouseActorState ASLMouseActor::GetCurrentState() const
{
	return CurrentState;
}

void ASLMouseActor::TakeDamage(float DamageAmount)
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
	TakeDamage(DamageAmount);
}

void ASLMouseActor::UpdateOrbitMovement(float DeltaTime)
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	// Update orbit center to follow player
	OrbitCenter = TargetPlayer->GetActorLocation();
	
	// Update orbit angle
	OrbitAngle += OrbitSpeed * DeltaTime;
	if (OrbitAngle >= 360.0f)
	{
		OrbitAngle -= 360.0f;
	}

	// Calculate new position in orbit
	float RadianAngle = FMath::DegreesToRadians(OrbitAngle);
	FVector OrbitPosition = OrbitCenter;
	OrbitPosition.X += FMath::Cos(RadianAngle) * OrbitRadius;
	OrbitPosition.Y += FMath::Sin(RadianAngle) * OrbitRadius;
	OrbitPosition.Z += OrbitHeight;

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

	// Move towards player
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

	// Move towards grab target location
	FVector MovementDelta = DirectionToTarget * GrabMoveSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + MovementDelta;
	SetActorLocation(NewLocation);

	// Check if we've reached the target location
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
	
	// Calculate direction to player
	FVector DirectionToPlayer = (PlayerLocation - MeshLocation).GetSafeNormal();
	
	// Create rotation that tilts the mesh 45 degrees toward the player
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MeshLocation, PlayerLocation);
	
	// Add 45 degree tilt (pitch down toward player)
	LookAtRotation.Pitch -= 45.0f;
	LookAtRotation.Roll = 90.0f;
	// Smoothly interpolate to the target rotation
	FRotator CurrentRotation = MouseMeshComponent->GetComponentRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 3.0f);
	
	MouseMeshComponent->SetWorldRotation(NewRotation);
}

void ASLMouseActor::StartGrabPlayer()
{
	if (!IsValid(TargetPlayer) || CurrentState != EMouseActorState::Descending || !CanGrabPlayer())
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Grabbing);
	bCanGrab = false;

	// Calculate grab target location (high in the air, away from current position)
	FVector CurrentLocation = GetActorLocation();
	FVector DirectionFromPlayer = (CurrentLocation - TargetPlayer->GetActorLocation()).GetSafeNormal();
	GrabTargetLocation = CurrentLocation + (DirectionFromPlayer * GrabDistance);
	GrabTargetLocation.Z += GrabHeight;

	// Attach player to mouse actor
	TargetPlayer->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	TargetPlayer->CharacterDragged(true);
	
	// Disable player movement while grabbed
	if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
	{
		MovementComp->DisableMovement();
	}

	// Play grab effects
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

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor grabbed player during descent"));
}

void ASLMouseActor::CompleteGrabPlayer()
{
	if (CurrentState != EMouseActorState::Grabbing)
	{
		return;
	}

	// Detach player from mouse actor
	if (IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetPlayer->CharacterDragged(false);
		// Re-enable player movement
		if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
		{
			MovementComp->SetDefaultMovementMode();
		}
	}

	// Temporarily disable collision to prevent immediate re-grab
	if (IsValid(CollisionComponent))
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		// Re-enable collision after delay
		GetWorldTimerManager().SetTimer(
			CollisionTimerHandle,
			[this]()
			{
				if (IsValid(CollisionComponent))
				{
					CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					UE_LOG(LogTemp, Log, TEXT("Mouse Actor collision re-enabled"));
				}
			},
			CollisionDisableTime,
			false
		);
	}

	// Apply damage to player
	ApplyGrabDamage();

	// Start grab cooldown
	float RandomCooldown = GetRandomGrabCooldown();
	GetWorldTimerManager().SetTimer(
		GrabCooldownTimerHandle,
		[this]()
		{
			bCanGrab = true;
			UE_LOG(LogTemp, Log, TEXT("Mouse Actor grab cooldown finished"));
		},
		RandomCooldown,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor grab cooldown started: %.1f seconds"), RandomCooldown);

	// Return to orbiting state
	SetMouseActorState(EMouseActorState::Orbiting);
	
	// Reset orbit position above player
	if (IsValid(TargetPlayer))
	{
		OrbitCenter = TargetPlayer->GetActorLocation();
		OrbitAngle = 0.0f;
		
		FVector OrbitPosition = OrbitCenter;
		OrbitPosition.X += OrbitRadius;
		OrbitPosition.Z += OrbitHeight;
		SetActorLocation(OrbitPosition);
	}

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor completed grab and returned to orbiting"));
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
		
		UE_LOG(LogTemp, Log, TEXT("Mouse Actor applied grab damage: %f"), GrabDamage);
	}
}

void ASLMouseActor::SetMouseActorState(EMouseActorState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		UE_LOG(LogTemp, Log, TEXT("Mouse Actor state changed to: %d"), (int32)NewState);
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