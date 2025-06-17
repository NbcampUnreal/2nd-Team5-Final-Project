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
#include "Engine/Engine.h"

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
	ChaseSpeed = 600.0f;
	GrabDistance = 500.0f;
	GrabHeight = 300.0f;
	GrabDamage = 20.0f;
	GrabDuration = 1.5f;
	GrabCooldown = 3.0f;
	MaxHealth = 30.0f;
	DetectionRange = 100.0f;

	// Runtime values
	CurrentState = EMouseActorState::Inactive;
	CurrentHealth = MaxHealth;
	TargetPlayer = nullptr;
	GrabStartLocation = FVector::ZeroVector;
	GrabTargetLocation = FVector::ZeroVector;
	bCanGrab = true;

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
}

void ASLMouseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	// Update target if needed
	UpdateChaseTarget();

	if (CurrentState == EMouseActorState::Chasing && IsValid(TargetPlayer))
	{
		MoveTowardsPlayer(DeltaTime);
		
		// Check if player is in grab range and can grab
		if (IsPlayerInRange() && CanGrabPlayer())
		{
			StartGrabPlayer();
		}
	}
	else if (CurrentState == EMouseActorState::Grabbing)
	{
		// Move towards grab target location while holding player
		MoveTowardsGrabTarget(DeltaTime);
	}

	// Update mesh rotation to look at player
	if (IsValid(TargetPlayer))
	{
		UpdateMeshRotation(DeltaTime);
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
	
	UE_LOG(LogTemp, Log, TEXT("Mouse Actor started chasing player"));
}

void ASLMouseActor::StopChasing()
{
	// Release player if currently grabbing
	if (CurrentState == EMouseActorState::Grabbing && IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
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
		GetWorldTimerManager().ClearTimer(GrabTimerHandle);
		GetWorldTimerManager().ClearTimer(GrabCooldownTimerHandle);
	}

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor stopped chasing"));
}

void ASLMouseActor::DestroyMouseActor()
{
	if (CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	// Release player if currently grabbing
	if (CurrentState == EMouseActorState::Grabbing && IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		// Re-enable player movement
		if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
		{
			MovementComp->SetDefaultMovementMode();
		}
	}

	SetMouseActorState(EMouseActorState::Destroyed);

	// Play destroy effects
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
		GetWorldTimerManager().ClearTimer(GrabTimerHandle);
		GetWorldTimerManager().ClearTimer(GrabCooldownTimerHandle);
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

void ASLMouseActor::SetChaseSpeed(float NewSpeed)
{
	ChaseSpeed = FMath::Max(0.0f, NewSpeed);
}

void ASLMouseActor::SetGrabSettings(float NewGrabDistance, float NewGrabHeight, float NewGrabDamage, float NewGrabDuration, float NewGrabCooldown)
{
	GrabDistance = FMath::Max(0.0f, NewGrabDistance);
	GrabHeight = FMath::Max(0.0f, NewGrabHeight);
	GrabDamage = FMath::Max(0.0f, NewGrabDamage);
	GrabDuration = FMath::Max(0.1f, NewGrabDuration);
	GrabCooldown = FMath::Max(0.1f, NewGrabCooldown);
}

void ASLMouseActor::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor) || CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	if (ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(OtherActor))
	{
		if (IsValid(PlayerCharacter) && CurrentState == EMouseActorState::Chasing && CanGrabPlayer())
		{
			StartGrabPlayer();
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
	FVector DirectionToPlayer = (PlayerLocation - CurrentLocation).GetSafeNormal();
	
	if (DirectionToPlayer.IsZero())
	{
		return;
	}

	// Move towards player
	FVector MovementDelta = DirectionToPlayer * ChaseSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + MovementDelta;
	SetActorLocation(NewLocation);

	// Rotate to face player
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, PlayerLocation);
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, 5.0f);
	SetActorRotation(NewRotation);
}

void ASLMouseActor::MoveTowardsGrabTarget(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector DirectionToTarget = (GrabTargetLocation - CurrentLocation).GetSafeNormal();
	
	if (DirectionToTarget.IsZero())
	{
		return;
	}

	// Move towards grab target location
	FVector MovementDelta = DirectionToTarget * ChaseSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + MovementDelta;
	SetActorLocation(NewLocation);

	// Rotate towards target
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, GrabTargetLocation);
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, 5.0f);
	SetActorRotation(NewRotation);

	// 목표 위치에 도달했는지 확인
	float DistanceToTarget = FVector::Dist(CurrentLocation, GrabTargetLocation);
	if (DistanceToTarget <= 50.0f) // 충분한 임계값에 근접
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
	
	// Smoothly interpolate to the target rotation
	FRotator CurrentRotation = MouseMeshComponent->GetComponentRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 3.0f);
	
	MouseMeshComponent->SetWorldRotation(NewRotation);
}

void ASLMouseActor::StartGrabPlayer()
{
	if (!IsValid(TargetPlayer) || CurrentState != EMouseActorState::Chasing || !CanGrabPlayer())
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Grabbing);
	bCanGrab = false;

	// Calculate grab target location (high in the air, away from current position)
	GrabStartLocation = GetActorLocation();
	FVector DirectionFromPlayer = (GrabStartLocation - TargetPlayer->GetActorLocation()).GetSafeNormal();
	GrabTargetLocation = GrabStartLocation + (DirectionFromPlayer * GrabDistance);
	GrabTargetLocation.Z += GrabHeight; // Move to higher location

	// Attach player to mouse actor
	TargetPlayer->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

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

	// Set backup timer in case something goes wrong with movement
	GetWorldTimerManager().SetTimer(
		GrabTimerHandle,
		this,
		&ASLMouseActor::CompleteGrabPlayer,
		GrabDuration * 2.0f, // Backup timer - twice the expected duration
		false
	);

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor grabbed player"));
}

void ASLMouseActor::CompleteGrabPlayer()
{
	if (CurrentState != EMouseActorState::Grabbing)
	{
		return;
	}

	// Clear the backup timer
	if (IsValid(GetWorld()))
	{
		GetWorldTimerManager().ClearTimer(GrabTimerHandle);
	}

	// Detach player from mouse actor
	if (IsValid(TargetPlayer))
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		// Re-enable player movement
		if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
		{
			MovementComp->SetDefaultMovementMode();
		}
	}

	// Apply damage to player
	ApplyGrabDamage();

	// Start grab cooldown
	GetWorldTimerManager().SetTimer(
		GrabCooldownTimerHandle,
		[this]()
		{
			bCanGrab = true;
			UE_LOG(LogTemp, Log, TEXT("Mouse Actor grab cooldown finished"));
		},
		GrabCooldown,
		false
	);

	// Return to chasing state
	SetMouseActorState(EMouseActorState::Chasing);

	UE_LOG(LogTemp, Log, TEXT("Mouse Actor completed grab and released player - cooldown started"));
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
	return bCanGrab && CurrentState == EMouseActorState::Chasing;
}