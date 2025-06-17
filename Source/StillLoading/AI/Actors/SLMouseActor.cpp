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
#include "Character/DataAsset/AttackDataAsset.h"
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
	CurrentHealth = MaxHealth;
	CurrentState = EMouseActorState::Inactive;
	TargetPlayer = nullptr;
	LastPlayerLocation = FVector::ZeroVector;
	TargetLocation = FVector::ZeroVector;
	bCanAttack = true;
	bIsGrabbing = false;

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

	if (CurrentState == EMouseActorState::Chasing || CurrentState == EMouseActorState::Attacking)
	{
		UpdateChaseTarget();
		MoveTowardsPlayer(DeltaTime);
		
		if (IsPlayerInAttackRange() && bCanAttack)
		{
			AttackPlayer();
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

	// Start threat effects
	GetWorldTimerManager().SetTimer(
		ThreatEffectTimerHandle,
		this,
		&ASLMouseActor::UpdateThreatEffects,
		ThreatEffectInterval,
		true
	);

	// Play chase effect
	if (ChaseEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ChaseEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// Play chase sound
	if (ChaseSound)
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
	
	// Clear timers
	GetWorldTimerManager().ClearTimer(ThreatEffectTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackCooldownHandle);

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

	// Clear all timers
	GetWorldTimerManager().ClearTimer(ThreatEffectTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackCooldownHandle);

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
	if (!TargetPlayer || CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	SetMouseActorState(EMouseActorState::Grabbing);
	TargetLocation = NewLocation;
	bIsGrabbing = true;

	// Move player using character movement if available
	if (UCharacterMovementComponent* MovementComp = TargetPlayer->GetCharacterMovement())
	{
		FVector LaunchDirection = (NewLocation - TargetPlayer->GetActorLocation()).GetSafeNormal();
		FVector LaunchVelocity = LaunchDirection * GrabForce;
		
		MovementComp->Launch(LaunchVelocity);
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
	if (!OtherActor || CurrentState == EMouseActorState::Destroyed)
	{
		return;
	}

	if (ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(OtherActor))
	{
		AttackPlayer();
	}
}

void ASLMouseActor::OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType)
{
	TakeDamage(DamageAmount);
}

void ASLMouseActor::UpdateChaseTarget()
{
	if (!TargetPlayer)
	{
		TargetPlayer = FindPlayerCharacter();
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

	// Apply damage through BattleComponent
	if (BattleComponent && TargetPlayer->FindComponentByClass<UBattleComponent>())
	{
		FHitResult HitResult;
		HitResult.ImpactPoint = TargetPlayer->GetActorLocation();
		HitResult.Location = TargetPlayer->GetActorLocation();
		HitResult.ImpactNormal = (GetActorLocation() - TargetPlayer->GetActorLocation()).GetSafeNormal();
		
		BattleComponent->SendHitResult(TargetPlayer, HitResult, EAttackAnimType::AAT_AIProjectile);
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
	if (ThreatEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ThreatEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	// Change material temporarily
	if (MouseMeshComponent && ThreatMaterial)
	{
		MouseMeshComponent->SetMaterial(0, ThreatMaterial);
		
		// Reset to normal material after a short time
		FTimerHandle MaterialResetHandle;
		GetWorldTimerManager().SetTimer(
			MaterialResetHandle,
			[this]() 
			{ 
				if (MouseMeshComponent && NormalMaterial)
				{
					MouseMeshComponent->SetMaterial(0, NormalMaterial);
				}
			},
			0.5f,
			false
		);
	}
}

void ASLMouseActor::PlayDestroyEffects()
{
	if (!GetWorld())
	{
		return;
	}

	if (DestroyEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (DestroySound)
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
	if (!TargetPlayer)
	{
		return;
	}

	FVector PlayerLocation = TargetPlayer->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	FVector DirectionToPlayer = (PlayerLocation - CurrentLocation).GetSafeNormal();

	// Move towards player
	FVector NewLocation = CurrentLocation + (DirectionToPlayer * ChaseSpeed * DeltaTime);
	SetActorLocation(NewLocation);

	// Rotate to face player
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, PlayerLocation);
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, RotationSpeed);
	SetActorRotation(NewRotation);

	LastPlayerLocation = PlayerLocation;
}

ASLPlayerCharacter* ASLMouseActor::FindPlayerCharacter()
{
	if (!GetWorld())
	{
		return nullptr;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return nullptr;
	}

	return Cast<ASLPlayerCharacter>(PlayerController->GetPawn());
}

bool ASLMouseActor::IsPlayerInAttackRange()
{
	if (!TargetPlayer)
	{
		return false;
	}

	float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
	return DistanceToPlayer <= AttackRange;
}

void ASLMouseActor::HandlePlayerGrabbing()
{
	if (!bIsGrabbing || !TargetPlayer)
	{
		return;
	}

	// Continue following player during grab state
	MoveTowardsPlayer(GetWorld()->GetDeltaSeconds());
}

void ASLMouseActor::ResetAttackCooldown()
{
	bCanAttack = true;
	SetMouseActorState(EMouseActorState::Chasing);
}