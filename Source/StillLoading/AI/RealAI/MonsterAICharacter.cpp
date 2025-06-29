#include "MonsterAICharacter.h"

#include "Boid/SwarmManager.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/Item/ArrowProjectile.h"
#include "Character/Item/SpearProjectile.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Controller/MonsterAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "Controller/SLBasePlayerController.h"

AMonsterAICharacter::AMonsterAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentHealth = MaxHealth;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AMonsterAIController::StaticClass();

	AnimationComponent = CreateDefaultSubobject<UAnimationMontageComponent>(TEXT("AnimationComponent"));
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
	SpawnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpawnTimeline"));

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMonsterAICharacter::OnHitByCharacter);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement();
		MoveComp && MoveComp->MovementMode == EMovementMode::MOVE_None)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
}

void AMonsterAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (SwordClass)
	{
		Sword = GetWorld()->SpawnActor<AActor>(SwordClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Sword, TEXT("hand_rSocket"));
		Sword->SetOwner(this);
	}

	if (ShieldClass)
	{
		Shield = GetWorld()->SpawnActor<AActor>(ShieldClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Shield, TEXT("hand_lSocket"));
		Shield->SetOwner(this);
	}

	BattleComponent->OnCharacterHited.AddDynamic(this, &AMonsterAICharacter::OnHitReceived);

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 150.f;

	if (!bOriginalMaterialsInitialized)
	{
		const USkeletalMeshComponent* MeshComp = GetMesh();
		const int32 MaterialCount = MeshComp->GetNumMaterials();

		OriginalMaterials.Empty();
		for (int32 i = 0; i < MaterialCount; ++i)
		{
			OriginalMaterials.Add(MeshComp->GetMaterial(i));
		}

		bOriginalMaterialsInitialized = true;
	}

	if (SpawnMovementCurve)
	{
		FOnTimelineFloat InterpFunction;
		InterpFunction.BindUFunction(this, FName("UpdateSpawnMovement"));

		FOnTimelineEvent TimelineFinishedFunction;
		TimelineFinishedFunction.BindUFunction(this, FName("OnSpawnMovementFinished"));

		SpawnTimeline->AddInterpFloat(SpawnMovementCurve, InterpFunction);
		SpawnTimeline->SetTimelineFinishedFunc(TimelineFinishedFunction);
	}

	SetPrimaryState(TAG_AI_Idle);
	SetStrategyState(TAG_AI_Idle);
	SetBattleState(TAG_AI_Idle);
}

void AMonsterAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMonsterAICharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Sword)
	{
		Sword->Destroy();
	}

	if (Shield)
	{
		Shield->Destroy();
	}
}

void AMonsterAICharacter::BeginSpawning(const FVector& FinalLocation, const float RiseHeight)
{
	SpawnEndLocation = FinalLocation;
	SpawnStartLocation = FinalLocation - FVector(0.f, 0.f, RiseHeight);

	SetActorLocation(SpawnStartLocation);
	AnimationComponent->PlayAIETCMontage("Spawn");

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		const float RandomMaxSpeed = FMath::FRandRange(200.f, 400.f);
		MoveComp->MaxWalkSpeed = RandomMaxSpeed;
	}

	//SetActorEnableCollision(false);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}

	ChangeMeshTemporarily(3);
	SpawnTimeline->PlayFromStart();
}

void AMonsterAICharacter::UpdateSpawnMovement(float Alpha)
{
	const FVector NewLocation = FMath::Lerp(SpawnStartLocation, SpawnEndLocation, Alpha);
	SetActorLocation(NewLocation);
}

void AMonsterAICharacter::OnSpawnMovementFinished()
{
	//SetActorEnableCollision(true);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AMonsterAICharacter::ToggleWeaponState(const bool bIsVisible)
{
	const bool bShouldBeHidden = !bIsVisible;

	if (Sword)
	{
		Sword->SetActorHiddenInGame(bShouldBeHidden);
	}

	if (Shield)
	{
		Shield->SetActorHiddenInGame(bShouldBeHidden);
	}
}

void AMonsterAICharacter::SpawnSpear()
{
	if (!ThrowableClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpearClass is not set in Character Blueprint!"));
		return;
	}

	const FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("hand_rSocket"));
	const FRotator SpawnRotation = GetActorRotation();

	const float RandomYaw = FMath::RandRange(-SpearInaccuracy, SpearInaccuracy);
	const float RandomPitch = FMath::RandRange(-SpearInaccuracy, SpearInaccuracy);
	const FRotator RandomOffset = FRotator(RandomPitch, RandomYaw, 0.0f);

	const FRotator FinalRotation = SpawnRotation + RandomOffset;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ToggleWeaponState(false);

	if (ASpearProjectile* SpawnedSpear = GetWorld()->SpawnActor<ASpearProjectile>(
		ThrowableClass, SpawnLocation, FinalRotation, SpawnParams))
	{
		UE_LOG(LogTemp, Log, TEXT("Spear Spawned!"));
	}
}

void AMonsterAICharacter::SpawnArrow()
{
	if (!ThrowableClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpearClass is not set in Character Blueprint!"));
		return;
	}

	const FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("hand_rSocket")) + FVector(0, 50, 0);
	const FRotator SpawnRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	if (AArrowProjectile* SpawnedArrow = GetWorld()->SpawnActor<AArrowProjectile>(
		ThrowableClass, SpawnLocation, SpawnRotation, SpawnParams))
	{
		UE_LOG(LogTemp, Log, TEXT("Spear Spawned!"));
	}
}

void AMonsterAICharacter::OnHitByCharacter(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(AMonsterAICharacter::StaticClass())
		|| OtherActor->IsA(ASLPlayerCharacter::StaticClass()))
	{
		if (!bRecentlyPushed)
		{
			bRecentlyPushed = true;
			FVector PushDirection = GetActorLocation() - OtherActor->GetActorLocation();
			PushDirection.Z = 0.0f;
			PushDirection.Normalize();

			LaunchCharacter(PushDirection * 500.0f, true, true);

			GetWorld()->GetTimerManager().SetTimer(PushResetHandle, this, &AMonsterAICharacter::ResetPushFlag, 0.5f,
			                                       false);

			ReportEnemy(OtherActor);
		}
	}
}

void AMonsterAICharacter::ResetPushFlag()
{
	bRecentlyPushed = false;
}

void AMonsterAICharacter::RotateToHitCauser(const AActor* Causer)
{
	if (!Causer) return;

	const FVector OwnerLocation = GetActorLocation();
	const FVector CauserLocation = Causer->GetActorLocation();

	const FVector ToCauser = (CauserLocation - OwnerLocation).GetSafeNormal2D();
	const FVector Forward = GetActorForwardVector().GetSafeNormal2D();

	const float Dot = FVector::DotProduct(Forward, ToCauser);

	FRotator TargetRotation = ToCauser.Rotation();
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;

	SetActorRotation(TargetRotation);
}

void AMonsterAICharacter::HitDirection(AActor* Causer)
{
	if (!Causer) return;

	const FVector OwnerLocation = GetActorLocation();
	const FVector CauserLocation = Causer->GetActorLocation();

	const FVector ToCauser = (CauserLocation - OwnerLocation).GetSafeNormal2D();
	const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
	const FVector Right = GetActorRightVector().GetSafeNormal2D();

	ForwardDot = FVector::DotProduct(Forward, ToCauser);
	RightDot = FVector::DotProduct(Right, ToCauser);
}

void AMonsterAICharacter::AttachItemToHand(AActor* ItemActor, const FName SocketName) const
{
	if (!ItemActor || !GetMesh()) return;

	ItemActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
}

void AMonsterAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonsterAICharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	StopFlyingState();
}

void AMonsterAICharacter::OnHoveredByCursor_Implementation(ASLBasePlayerController* InstigatingController)
{
	if (InstigatingController)
	{
		InstigatingController->SetEnemyCursor();
	}
}

void AMonsterAICharacter::OnUnhoveredByCursor_Implementation(ASLBasePlayerController* InstigatingController)
{
	if (InstigatingController)
	{
		InstigatingController->SetDefaultCursor();
	}
}

void AMonsterAICharacter::SetChasing(bool bEnable)
{
	bIsChasing = bEnable;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = bEnable ? 350.f : 350.f;
	}
}

void AMonsterAICharacter::SetPrimaryState(const FGameplayTag NewState)
{
	StateTags.Reset();
	StateTags.AddTag(NewState);
}

bool AMonsterAICharacter::IsInPrimaryState(const FGameplayTag StateToCheck) const
{
	return StateTags.HasTagExact(StateToCheck);
}

void AMonsterAICharacter::SetMonsterModeState(const FGameplayTag NewState)
{
	MonsterModeTags.Reset();
	MonsterModeTags.AddTag(NewState);
}

bool AMonsterAICharacter::HasMonsterModeState(const FGameplayTag StateToCheck) const
{
	return MonsterModeTags.HasTag(StateToCheck);
}

void AMonsterAICharacter::SetBattleState(const FGameplayTag NewState)
{
	BattleStateTags.Reset();
	BattleStateTags.AddTag(NewState);
}

bool AMonsterAICharacter::HasBattleState(const FGameplayTag StateToCheck) const
{
	return BattleStateTags.HasTag(StateToCheck);
}

void AMonsterAICharacter::SetStrategyState(const FGameplayTag NewState)
{
	StrategyStateTags.Reset();
	StrategyStateTags.AddTag(NewState);
}

bool AMonsterAICharacter::HasStrategyState(const FGameplayTag StateToCheck) const
{
	return StrategyStateTags.HasTag(StateToCheck);
}

void AMonsterAICharacter::RemoveStrategyState()
{
	StrategyStateTags.Reset();
}

void AMonsterAICharacter::OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult,
                                        EHitAnimType AnimType)
{
	if (HasBattleState(TAG_AI_Dead))
	{
		return;
	}

	ReportEnemy(Causer);
	AnimationComponent->StopAllMontages(0.2f);
	GetBattleSoundSubSystem()->PlayBattleSound(EBattleSoundType::BST_MonsterHit, GetActorLocation());
	
	LastAttacker = Causer;
	CurrentHealth -= Damage;

	UE_LOG(LogTemp, Warning, TEXT("Monster Current Health[%f]"), CurrentHealth);

	if (CurrentHealth <= 0.f)
	{
		SetBattleState(TAG_AI_Dead);
		AnimationComponent->PlayAIHitMontage("Dead");
		Dead(Causer, true);
		return;
	}

	if (Causer != nullptr && Causer != this)
	{
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController != nullptr)
		{
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;

			const FVector CauserLocation = Causer->GetActorLocation();
			const FVector DirectionToCauser = (CauserLocation - GetActorLocation()).GetSafeNormal();
			const FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToCauser).Rotator();

			AIController->SetControlRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
		}
	}

	HitDirection(Causer);
	RotateToHitCauser(Causer);
	//ChangeMeshTemporarily();
	StartFlyingState();

	switch (AnimType)
	{
	case EHitAnimType::HAT_WeakHit:
	case EHitAnimType::HAT_HardHit:
		{
			SetBattleState(TAG_AI_Hit);
			PlayHitMontageAndSetupRecovery(0.8);

			FVector KnockbackDir = GetActorLocation() - Causer->GetActorLocation();
			KnockbackDir.Z = 0;
			KnockbackDir.Normalize();

			const float GroundDistance = GetCharacterMovement()->CurrentFloor.FloorDist;
			if (GetCharacterMovement()->IsFalling() && GroundDistance > 20.0f)
			{
				LaunchCharacter(KnockbackDir * 1200, true, false);
			}
			else
			{
				LaunchCharacter(KnockbackDir * 1200, true, false);
			}

			break;
		}
	case EHitAnimType::HAT_AirBorne:
		{
			SetStrategyState(TAG_AI_IsPlayingMontage);
			AnimationComponent->PlayAIHitMontage("Airborne");
		}
		break;
	case EHitAnimType::HAT_AirUp:
		{
			SetStrategyState(TAG_AI_IsPlayingMontage);
			AnimationComponent->PlayAIHitMontage("AirUp");
		}
		break;
	case EHitAnimType::HAT_FallBack:
		RotateToHitCauser(Causer);
		{
			SetStrategyState(TAG_AI_IsPlayingMontage);
			AnimationComponent->PlayAIHitMontage("GroundHit");
		}
		break;
	case EHitAnimType::HAT_KillMotionA:
		AnimationComponent->PlayAIHitMontage("ExecutionA");
		SetBattleState(TAG_AI_Dead);
		break;
	case EHitAnimType::HAT_KillMotionB:
		AnimationComponent->PlayAIHitMontage("ExecutionB");
		SetBattleState(TAG_AI_Dead);
		break;
	case EHitAnimType::HAT_KillMotionC:
		AnimationComponent->PlayAIHitMontage("ExecutionC");
		SetBattleState(TAG_AI_Dead);
		break;
	case EHitAnimType::HAT_Parry:
		break;
	default: break;
	}
}

void AMonsterAICharacter::ChangeMeshTemporarily(const float Rate)
{
	if (!HitMaterial || !bOriginalMaterialsInitialized)
		return;

	USkeletalMeshComponent* MeshComp = GetMesh();

	if (GetWorld()->GetTimerManager().IsTimerActive(MaterialResetTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(MaterialResetTimerHandle);
	}

	for (int32 i = 0; i < OriginalMaterials.Num(); ++i)
	{
		MeshComp->SetMaterial(i, HitMaterial);
	}

	GetWorld()->GetTimerManager().SetTimer(
		MaterialResetTimerHandle,
		this,
		&AMonsterAICharacter::ResetMaterial,
		Rate,
		false
	);
}

void AMonsterAICharacter::ResetMaterial()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	for (int32 i = 0; i < OriginalMaterials.Num(); ++i)
	{
		MeshComp->SetMaterial(i, OriginalMaterials[i]);
	}
}

void AMonsterAICharacter::HandleAnimNotify(EAttackAnimType MonsterMontageStage)
{
	switch (MonsterMontageStage)
	{
	case EAttackAnimType::AAT_AINormal:
	case EAttackAnimType::AAT_AISpecial:
		LastAttackFinishTime = GetWorld()->GetTimeSeconds();
		break;
	case EAttackAnimType::AAT_FinalAttackA:
	case EAttackAnimType::AAT_FinalAttackB:
	case EAttackAnimType::AAT_FinalAttackC:
	case EAttackAnimType::AAT_Dead:
		GetBattleSoundSubSystem()->PlayBattleSound(EBattleSoundType::BST_MonsterDie, GetActorLocation());
		//Dead(LastAttacker, true);
		return;
	}

	if (HasBattleState(TAG_AI_Dead))
		return;

	SetPrimaryState(TAG_AI_Idle);
	SetStrategyState(TAG_AI_Idle);
	SetBattleState(TAG_AI_Idle);
	StopFlyingState();
}

void AMonsterAICharacter::HandleHitNotify()
{
	if (HasBattleState(TAG_AI_Dead))
		return;

	SetPrimaryState(TAG_AI_Idle);
	SetStrategyState(TAG_AI_Idle);
	SetBattleState(TAG_AI_Idle);
	StopFlyingState();
}

void AMonsterAICharacter::Dead(const AActor* Attacker, const bool bIsChangeMaterial)
{
	SetBattleState(TAG_AI_Dead);
	OnDeath();
	AgentDied();

	if (DeathMaterial && bIsChangeMaterial)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(MaterialResetTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(MaterialResetTimerHandle);
		}
		GetMesh()->SetMaterial(0, DeathMaterial);
	}

	if (AMonsterAIController* AICon = Cast<AMonsterAIController>(GetController()))
	{
		AICon->StopLogic("Character Died"); // StopLogic으로 변경
		AICon->UnPossess();
		AICon->ToggleLockOnWidget(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (Sword)
	{
		Sword->SetActorHiddenInGame(true);
	}
	if (Shield)
	{
		Shield->SetActorHiddenInGame(true);
	}
    
	FTimerHandle DeathTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AMonsterAICharacter::OnDeathSequenceFinished, 2.0f, false);

	if (Attacker)
	{
		if (UBattleComponent* AttackerBattleComp = Attacker->FindComponentByClass<UBattleComponent>())
		{
			IEnemyDeathReceiver::Execute_OnEnemyDeath(AttackerBattleComp, this);
		}
	}
}

void AMonsterAICharacter::OnDeathSequenceFinished()
{
	if (OnMonsterDied.IsBound())
	{
		OnMonsterDied.Broadcast(this);
	}
}

/*
void AMonsterAICharacter::Dead(const AActor* Attacker, const bool bIsChangeMaterial)
{
	OnDeath();
	AgentDied();

	if (DeathMaterial && bIsChangeMaterial)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(MaterialResetTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(MaterialResetTimerHandle);
		}

		GetMesh()->SetMaterial(0, DeathMaterial);
	}

	if (AMonsterAIController* AICon = Cast<AMonsterAIController>(GetController()))
	{
		AICon->StopMovement();
		AICon->UnPossess();
		AICon->ToggleLockOnWidget(false);
	}

	if (Sword)
	{
		Sword->Destroy();
		Sword = nullptr;
	}

	if (Shield)
	{
		Shield->Destroy();
		Shield = nullptr;
	}

	// 콜리전 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//FixCharacterVelocity();

	// 나중에 Destroy 또는 사라짐 처리
	SetLifeSpan(2.f);

	// BattleComponent에 전달
	if (Attacker)
	{
		if (UBattleComponent* AttackerBattleComp = Attacker->FindComponentByClass<UBattleComponent>())
		{
			IEnemyDeathReceiver::Execute_OnEnemyDeath(AttackerBattleComp, this);

			if (OnMonsterDied.IsBound())
			{
				OnMonsterDied.Broadcast(this);
			}
		}
	}
}
*/

void AMonsterAICharacter::FixCharacterVelocity()
{
	FVector Velocity = GetCharacterMovement()->Velocity;
	Velocity.Z = 0.f;
	GetCharacterMovement()->Velocity = Velocity;
}

void AMonsterAICharacter::StartFlyingState()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,
		                                                     ECollisionResponse::ECR_Ignore);
	}
}

void AMonsterAICharacter::StopFlyingState()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->
			SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	}
}

void AMonsterAICharacter::ReportEnemy(AActor* Causer)
{
	if (APawn* DetectedActor = Cast<APawn>(Causer))
	{
		CurrentDetectedActor = DetectedActor;
		ASwarmManager* SwarmManager = GetMySwarmManager();
		if (!SwarmManager) return;
		SwarmManager->ReportTargetSighting(this, CurrentDetectedActor);
	}
}

USLSoundSubsystem* AMonsterAICharacter::GetBattleSoundSubSystem() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<USLSoundSubsystem>();
		}
	}

	return nullptr;
}

void AMonsterAICharacter::RecoverFromHitState()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	SetBattleState(TAG_AI_Idle);
}

void AMonsterAICharacter::PlayHitMontageAndSetupRecovery(const float Length)
{
	if (Length > 0.f)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		if (TimerManager.IsTimerActive(CollisionResetTimerHandle))
		{
			return;
		}

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		//GetWorld()->GetTimerManager().ClearTimer(CollisionResetTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(CollisionResetTimerHandle, this,
		                                       &AMonsterAICharacter::RecoverFromHitState, Length, false);
	}
	else
	{
		RecoverFromHitState();
	}
}
