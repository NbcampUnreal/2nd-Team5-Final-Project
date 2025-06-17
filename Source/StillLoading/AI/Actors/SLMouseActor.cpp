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
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

ASLMouseActor::ASLMouseActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root Component Setup
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	// Mesh Component
	MouseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MouseMeshComponent"));
	MouseMeshComponent->SetupAttachment(RootComponent);
	MouseMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Collision Component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	// Battle Component
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));

	// Initialize values
	ChaseSpeed = 600.0f;
	AttackDamage = 15.0f;
	AttackRange = 150.0f;
	GrabForce = 1000.0f;
	MaxHealth = 100.0f;
	ThreatEffectInterval = 2.0f;
	RotationSpeed = 5.0f;
	AttackCooldown = 1.5f;
	
	// Periodic Grab Settings
	bEnablePeriodicGrab = true;
	GrabIntervalMin = 3.0f;
	GrabIntervalMax = 7.0f;
	GrabProbability = 0.7f; // 70% chance
	GrabLocationRadius = 800.0f;
	MinGrabDistance = 200.0f;
	MaxGrabDistance = 1000.0f;
	
	// Runtime values
	CurrentHealth = MaxHealth;
	CurrentState = EMouseActorState::Inactive;
	TargetPlayer = nullptr;
	LastPlayerLocation = FVector::ZeroVector;
	TargetLocation = FVector::ZeroVector;
	bCanAttack = true;
	bIsGrabbing = false;
	bPeriodicGrabActive = false;

	// Debug Settings
	bShowDebugInfo = false;
	bDrawDebugLines = false;
	DebugLineThickness = 3.0f;
	DebugLineDuration = 0.1f;

	// Effects and Materials
	ChaseEffect = nullptr;
	AttackEffect = nullptr;
	DestroyEffect = nullptr;
	ThreatEffect = nullptr;
	ChaseSound = nullptr;
	AttackSound = nullptr;
	DestroySound = nullptr;
	NormalMaterial = nullptr;
	ThreatMaterial = nullptr;
}

void ASLMouseActor::BeginPlay()
{
	Super::BeginPlay();

	// Setup collision events
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &ASLMouseActor::OnCollisionHit);
		CollisionComponent->SetSphereRadius(AttackRange);
	}

	// Setup battle component
	if (BattleComponent)
	{
		BattleComponent->OnCharacterHited.AddDynamic(this, &ASLMouseActor::OnBattleComponentHit);
	}

	// Find initial target
	TargetPlayer = FindPlayerCharacter();
}

void ASLMouseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Enhanced null checks for safer execution
	if (!IsValid(TargetPlayer))
	{
		TargetPlayer = FindPlayerCharacter();
		if (!IsValid(TargetPlayer))
		{
			if (bShowDebugInfo)
			{
				UE_LOG(LogTemp, Warning, TEXT("Mouse Actor: No valid player found!"));
			}
			return; // Exit early if no valid player found
		}
	}

	if (CurrentState == EMouseActorState::Chasing || CurrentState == EMouseActorState::Attacking)
	{
		UpdateChaseTarget();
		MoveTowardsPlayer(DeltaTime);
		
		if (IsPlayerInAttackRange() && bCanAttack)
		{
			AttackPlayer();
		}
		
		// Debug state information
		if (bShowDebugInfo)
		{
			static float DebugTimer = 0.0f;
			DebugTimer += DeltaTime;
			
			if (DebugTimer >= 1.0f) // Log every second
			{
				DebugTimer = 0.0f;
				float Distance = GetDistanceToPlayer();
				UE_LOG(LogTemp, Log, TEXT("Mouse Actor Status: State=%d, Distance=%.1f, CanAttack=%s, Target=%s"), 
					(int32)CurrentState, Distance, bCanAttack ? TEXT("Yes") : TEXT("No"), 
					IsValid(TargetPlayer) ? *TargetPlayer->GetName() : TEXT("None"));
			}
		}
	}
	else if (CurrentState == EMouseActorState::Grabbing)
	{
		HandlePlayerGrabbing();
	}
}

void ASLMouseActor::StartChasing()
{
	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Chasing);
	TargetPlayer = FindPlayerCharacter();
	
	if (!IsValid(TargetPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Mouse Actor: Cannot start chasing - no valid player found"));
		return;
	}

	// Start threat effects
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().SetTimer(
			ThreatEffectTimerHandle,
			this,
			&ASLMouseActor::UpdateThreatEffects,
			ThreatEffectInterval,
			true
		);
	}

	// Start periodic grab system
	if (bEnablePeriodicGrab)
	{
		StartPeriodicGrab();
	}

	// Play chase effect
	if (IsValid(ChaseEffect) && IsValid(GetWorld()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ChaseEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// Play chase sound
	if (IsValid(ChaseSound) && IsValid(GetWorld()))
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ChaseSound,
			GetActorLocation()
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor started chasing player"));
}

void ASLMouseActor::StopChasing()
{
	SetMouseActorState(EMouseActorState::Inactive);
	
	// Clear timers safely
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().ClearTimer(ThreatEffectTimerHandle);
		GetWorldTimerManager().ClearTimer(AttackCooldownHandle);
		GetWorldTimerManager().ClearTimer(PeriodicGrabTimerHandle);
	}

	// Stop periodic grab
	StopPeriodicGrab();

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor stopped chasing"));
}

void ASLMouseActor::DestroyMouseActor()
{
	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Destroyed);
	PlayDestroyEffects();

	// Clear all timers safely
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().ClearTimer(ThreatEffectTimerHandle);
		GetWorldTimerManager().ClearTimer(AttackCooldownHandle);
		GetWorldTimerManager().ClearTimer(PeriodicGrabTimerHandle);
	}

	// Stop periodic grab
	StopPeriodicGrab();

	// Broadcast destruction
	if (OnMouseActorDestroyed.IsBound())
	{
		OnMouseActorDestroyed.Broadcast(this);
	}

	// Hide and disable collision
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor destroyed"));
}

void ASLMouseActor::GrabAndMovePlayer(const FVector& NewLocation)
{
	if (!IsValid(TargetPlayer) || CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Grabbing);
	TargetLocation = NewLocation;
	bIsGrabbing = true;

	// More robust player movement handling
	if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
	{
		FVector LaunchDirection = (NewLocation - TargetPlayer->GetActorLocation()).GetSafeNormal();
		
		// Ensure we have a valid direction
		if (!LaunchDirection.IsZero())
		{
			FVector LaunchVelocity = LaunchDirection * GrabForce;
			MovementComp->Launch(LaunchVelocity);
		}
	}
	else
	{
		// Fallback: Try to move using physics or direct location setting
		if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(TargetPlayer->GetRootComponent()))
		{
			if (RootPrimitive->IsSimulatingPhysics())
			{
				FVector LaunchDirection = (NewLocation - TargetPlayer->GetActorLocation()).GetSafeNormal();
				if (!LaunchDirection.IsZero())
				{
					FVector LaunchVelocity = LaunchDirection * GrabForce;
					RootPrimitive->AddImpulse(LaunchVelocity, NAME_None, true);
				}
			}
		}
	}

	// Return to chasing after grab
	FTimerHandle GrabRecoveryHandle;
	GetWorldTimerManager().SetTimer(
		GrabRecoveryHandle,
		[this]() 
		{ 
			bIsGrabbing = false;
			SetMouseActorState(EMouseActorState::Chasing); 
		},
		2.0f,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor grabbed and moved player"));
}

void ASLMouseActor::SetChaseSpeed(float NewSpeed)
{
	ChaseSpeed = FMath::Max(0.0f, NewSpeed);
}

void ASLMouseActor::SetAttackDamage(float NewDamage)
{
	AttackDamage = FMath::Max(0.0f, NewDamage);
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

void ASLMouseActor::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor) || CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	if (ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(OtherActor))
	{
		if (IsValid(PlayerCharacter))
		{
			AttackPlayer();
		}
	}
}

void ASLMouseActor::OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType)
{
	TakeDamage(DamageAmount);
}

void ASLMouseActor::UpdateChaseTarget()
{
	if (!IsValid(TargetPlayer))
	{
		TargetPlayer = FindPlayerCharacter();
		if (!IsValid(TargetPlayer))
		{
			UE_LOG(LogTemp, Warning, TEXT("Mouse Actor: Unable to find valid player target"));
		}
	}
}

void ASLMouseActor::ChasePlayer()
{
	if (!TargetPlayer)
	{
		return;
	}

	FVector PlayerLocation = TargetPlayer->GetActorLocation();
	LastPlayerLocation = PlayerLocation;
}

void ASLMouseActor::AttackPlayer()
{
	if (!TargetPlayer || !bCanAttack || CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Attacking);
	bCanAttack = false;

	// Apply damage through BattleComponent with enhanced null checks
	if (IsValid(TargetPlayer))
	{
		if (UBattleComponent* PlayerBattleComp = TargetPlayer->FindComponentByClass<UBattleComponent>())
		{
			FHitResult HitResult;
			HitResult.ImpactPoint = TargetPlayer->GetActorLocation();
			HitResult.Location = TargetPlayer->GetActorLocation();
			HitResult.ImpactNormal = (GetActorLocation() - TargetPlayer->GetActorLocation()).GetSafeNormal();
			HitResult.Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
			HitResult.TraceStart = GetActorLocation();
			HitResult.TraceEnd = TargetPlayer->GetActorLocation();
			
			// Use ReceiveHitResult directly instead of SendHitResult to avoid Owner issues
			PlayerBattleComp->ReceiveHitResult(AttackDamage, this, HitResult, EAttackAnimType::AAT_AISpecial);
			
			UE_LOG(LogTemp, Log, TEXT("Mouse Actor applied damage: %f to %s"), AttackDamage, *TargetPlayer->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Target player does not have BattleComponent"));
		}
	}

	// Play attack effects
	if (AttackEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			AttackEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			AttackSound,
			GetActorLocation()
		);
	}

	// Set attack cooldown
	GetWorldTimerManager().SetTimer(
		AttackCooldownHandle,
		this,
		&ASLMouseActor::ResetAttackCooldown,
		AttackCooldown,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor attacked player"));
}

void ASLMouseActor::UpdateThreatEffects()
{
	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	// Spawn threat effect
	if (IsValid(ThreatEffect) && IsValid(GetWorld()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ThreatEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// Change material temporarily
	if (IsValid(MouseMeshComponent) && IsValid(ThreatMaterial))
	{
		MouseMeshComponent->SetMaterial(0, ThreatMaterial);
		
		// Reset to normal material after a short time
		if (IsValid(GetWorld()))
		{
			FTimerHandle MaterialResetHandle;
			GetWorldTimerManager().SetTimer(
				MaterialResetHandle,
				[this]() 
				{ 
					if (IsValid(MouseMeshComponent) && IsValid(NormalMaterial))
					{
						MouseMeshComponent->SetMaterial(0, NormalMaterial);
					}
				},
				0.5f,
				false
			);
		}
	}
}

void ASLMouseActor::PlayDestroyEffects()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	if (IsValid(DestroyEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (IsValid(DestroySound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			DestroySound,
			GetActorLocation()
		);
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

void ASLMouseActor::MoveTowardsPlayer(float DeltaTime)
{
	if (!IsValid(TargetPlayer))
	{
		return;
	}

	FVector PlayerLocation = TargetPlayer->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	
	// Check for valid locations
	if (PlayerLocation.IsZero() || CurrentLocation.IsZero())
	{
		return;
	}
	
	FVector DirectionToPlayer = (PlayerLocation - CurrentLocation).GetSafeNormal();
	
	// Ensure we have a valid direction
	if (DirectionToPlayer.IsZero())
	{
		return;
	}

	// Calculate movement
	FVector MovementDelta = DirectionToPlayer * ChaseSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + MovementDelta;
	
	// Apply movement
	SetActorLocation(NewLocation);

	// Rotate to face player
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, PlayerLocation);
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, RotationSpeed);
	SetActorRotation(NewRotation);

	LastPlayerLocation = PlayerLocation;

	// Debug Information
	if (bShowDebugInfo)
	{
		float DistanceToPlayer = FVector::Dist(CurrentLocation, PlayerLocation);
		UE_LOG(LogTemp, Warning, TEXT("MouseActor Moving: Pos(%s) → Player(%s), Distance=%.1f, Speed=%.1f"), 
			*CurrentLocation.ToString(), *PlayerLocation.ToString(), DistanceToPlayer, ChaseSpeed);
	}

	// Debug Visual Lines
	if (bDrawDebugLines && IsValid(GetWorld()))
	{
		// Draw line from mouse to player
		DrawDebugLine(
			GetWorld(),
			CurrentLocation,
			PlayerLocation,
			FColor::Red,
			false,
			DebugLineDuration,
			0,
			DebugLineThickness
		);

		// Draw movement vector
		DrawDebugLine(
			GetWorld(),
			CurrentLocation,
			NewLocation,
			FColor::Green,
			false,
			DebugLineDuration,
			0,
			DebugLineThickness * 2.0f
		);

		// Draw mouse position sphere
		DrawDebugSphere(
			GetWorld(),
			CurrentLocation,
			25.0f,
			8,
			FColor::Yellow,
			false,
			DebugLineDuration
		);

		// Draw player position sphere
		DrawDebugSphere(
			GetWorld(),
			PlayerLocation,
			30.0f,
			8,
			FColor::Blue,
			false,
			DebugLineDuration
		);
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

	ASLPlayerCharacter* PlayerChar = Cast<ASLPlayerCharacter>(PlayerPawn);
	return PlayerChar; // Cast can return nullptr, which is fine
}

bool ASLMouseActor::IsPlayerInAttackRange()
{
	if (!IsValid(TargetPlayer))
	{
		return false;
	}

	float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
	return DistanceToPlayer <= AttackRange && DistanceToPlayer > 0.0f;
}

void ASLMouseActor::HandlePlayerGrabbing()
{
	if (!bIsGrabbing || !IsValid(TargetPlayer))
	{
		return;
	}

	// Continue following player during grab state with enhanced safety
	if (IsValid(GetWorld()))
	{
		MoveTowardsPlayer(GetWorld()->GetDeltaSeconds());
	}
}

void ASLMouseActor::ResetAttackCooldown()
{
	bCanAttack = true;
	SetMouseActorState(EMouseActorState::Chasing);
}

void ASLMouseActor::EnablePeriodicGrab(bool bEnable)
{
	bEnablePeriodicGrab = bEnable;
	
	if (bEnable && CurrentState == EMouseActorState::Chasing)
	{
		StartPeriodicGrab();
	}
	else if (!bEnable)
	{
		StopPeriodicGrab();
	}
}

void ASLMouseActor::SetGrabSettings(float MinInterval, float MaxInterval, float GrabChance, float GrabRadius)
{
	GrabIntervalMin = FMath::Max(0.5f, MinInterval);
	GrabIntervalMax = FMath::Max(GrabIntervalMin, MaxInterval);
	GrabProbability = FMath::Clamp(GrabChance, 0.0f, 1.0f);
	GrabLocationRadius = FMath::Max(100.0f, GrabRadius);
}

void ASLMouseActor::StartPeriodicGrab()
{
	if (!bEnablePeriodicGrab || bPeriodicGrabActive || CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	bPeriodicGrabActive = true;
	ScheduleNextGrab();
	
	UE_LOG(LogTemp, Log, TEXT("Mouse Actor: Periodic grab system started"));
}

void ASLMouseActor::StopPeriodicGrab()
{
	if (!bPeriodicGrabActive)
	{
		return;
	}

	bPeriodicGrabActive = false;
	
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().ClearTimer(PeriodicGrabTimerHandle);
	}
	
	UE_LOG(LogTemp, Log, TEXT("Mouse Actor: Periodic grab system stopped"));
}

void ASLMouseActor::ExecutePeriodicGrab()
{
	if (!bPeriodicGrabActive || CurrentState == EMouseActorState::Destroyed || bIsGrabbing)
	{
		return;
	}

	// Check probability
	float RandomChance = FMath::FRand();
	if (RandomChance > GrabProbability)
	{
		UE_LOG(LogTemp, Log, TEXT("Mouse Actor: Grab skipped due to probability (%.2f > %.2f)"), RandomChance, GrabProbability);
		ScheduleNextGrab();
		return;
	}

	// Get random grab location
	FVector GrabLocation = GetRandomGrabLocation();
	
	if (IsSafeGrabLocation(GrabLocation))
	{
		UE_LOG(LogTemp, Log, TEXT("Mouse Actor: Executing periodic grab to location: %s"), *GrabLocation.ToString());
		GrabAndMovePlayer(GrabLocation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Mouse Actor: Unsafe grab location, skipping"));
	}

	// Schedule next grab
	ScheduleNextGrab();
}

FVector ASLMouseActor::GetRandomGrabLocation()
{
	if (!IsValid(TargetPlayer))
	{
		return GetActorLocation();
	}

	FVector PlayerLocation = TargetPlayer->GetActorLocation();
	
	// Generate random angle and distance
	float RandomAngle = FMath::FRandRange(0.0f, 360.0f);
	float RandomDistance = FMath::FRandRange(MinGrabDistance, MaxGrabDistance);
	
	// Calculate direction from angle
	FVector RandomDirection = FVector(
		FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
		FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
		0.0f
	);
	
	// Calculate final location
	FVector GrabLocation = PlayerLocation + (RandomDirection * RandomDistance);
	
	// Add some height variation
	float HeightVariation = FMath::FRandRange(-100.0f, 200.0f);
	GrabLocation.Z += HeightVariation;
	
	return GrabLocation;
}

bool ASLMouseActor::IsSafeGrabLocation(const FVector& Location)
{
	if (!IsValid(GetWorld()))
	{
		return false;
	}

	// Basic bounds check
	if (Location.Z < -1000.0f || Location.Z > 2000.0f)
	{
		return false;
	}

	// Check if location is too close to current player position
	if (IsValid(TargetPlayer))
	{
		float DistanceToPlayer = FVector::Dist(Location, TargetPlayer->GetActorLocation());
		if (DistanceToPlayer < MinGrabDistance)
		{
			return false;
		}
	}

	// Optional: Add more sophisticated checks like ground collision, obstacles, etc.
	// For now, we'll keep it simple

	return true;
}

void ASLMouseActor::ScheduleNextGrab()
{
	if (!bPeriodicGrabActive || !IsValid(GetWorld()))
	{
		return;
	}

	// Calculate random interval
	float NextGrabInterval = FMath::FRandRange(GrabIntervalMin, GrabIntervalMax);
	
	// Set timer for next grab
	GetWorldTimerManager().SetTimer(
		PeriodicGrabTimerHandle,
		this,
		&ASLMouseActor::ExecutePeriodicGrab,
		NextGrabInterval,
		false
	);
	
	UE_LOG(LogTemp, Log, TEXT("Mouse Actor: Next grab scheduled in %.1f seconds"), NextGrabInterval);
}

void ASLMouseActor::SetDebugMode(bool bEnable)
{
	bShowDebugInfo = bEnable;
	bDrawDebugLines = bEnable;
	
	UE_LOG(LogTemp, Warning, TEXT("Mouse Actor Debug Mode: %s"), bEnable ? TEXT("ENABLED") : TEXT("DISABLED"));
}

FVector ASLMouseActor::GetPlayerDirection() const
{
	if (!IsValid(TargetPlayer))
	{
		return FVector::ZeroVector;
	}

	FVector CurrentLocation = GetActorLocation();
	FVector PlayerLocation = TargetPlayer->GetActorLocation();
	
	return (PlayerLocation - CurrentLocation).GetSafeNormal();
}

float ASLMouseActor::GetDistanceToPlayer() const
{
	if (!IsValid(TargetPlayer))
	{
		return -1.0f;
	}

	return FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
}