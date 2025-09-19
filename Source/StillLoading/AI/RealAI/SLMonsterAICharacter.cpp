#include "SLMonsterAICharacter.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/Item/ArrowProjectile.h"
#include "Character/Item/SpearProjectile.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Component/SLAICombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "Controller/SLBasePlayerController.h"
#include "Controller/SLMonsterAIController.h"
#include "Spawner/SLSwarmSpawner.h"

ASLMonsterAICharacter::ASLMonsterAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ASLMonsterAIController::StaticClass();

	SpawnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpawnTimeline"));

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASLMonsterAICharacter::OnHitByCharacter);
}

void ASLMonsterAICharacter::BeginPlay()
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

	BattleComponent->OnCharacterHited.AddDynamic(this, &ASLMonsterAICharacter::OnHitReceived);

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
}

void ASLMonsterAICharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void ASLMonsterAICharacter::PlayAttackAnim()
{
	TArray<FString> AttackMontageNames = {"Attack1", "Attack2", "Attack3"};
	const int32 RandIndex = FMath::RandRange(0, AttackMontageNames.Num() - 1);
	AnimationComponent->PlayAIAttackMontage(*AttackMontageNames[RandIndex]);
}

void ASLMonsterAICharacter::PlayETCAnim()
{
	TArray<FString> AttackMontageNames = {"WonderA", "WonderB", "WonderC", "WonderD", "WonderE"};
	const int32 RandIndex = FMath::RandRange(0, AttackMontageNames.Num() - 1);
	AnimationComponent->PlayAIETCMontage(*AttackMontageNames[RandIndex]);
}

void ASLMonsterAICharacter::PlayETCWaitAnim()
{
	TArray<FString> AttackMontageNames = {"WaitA", "WaitB", "WaitC", "WaitD"};
	const int32 RandIndex = FMath::RandRange(0, AttackMontageNames.Num() - 1);
	AnimationComponent->PlayAIETCMontage(*AttackMontageNames[RandIndex]);
}

void ASLMonsterAICharacter::BeginSpawning(const FVector& FinalLocation, const float RiseHeight)
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

void ASLMonsterAICharacter::UpdateSpawnMovement(float Alpha)
{
	const FVector NewLocation = FMath::Lerp(SpawnStartLocation, SpawnEndLocation, Alpha);
	SetActorLocation(NewLocation);
}

void ASLMonsterAICharacter::OnSpawnMovementFinished() const
{
	//SetActorEnableCollision(true);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void ASLMonsterAICharacter::ToggleWeaponState(const bool bIsVisible)
{
	const bool bShouldBeHidden = !bIsVisible;

	if (Sword)
	{
		Sword->SetActorHiddenInGame(bShouldBeHidden);
		Sword->SetActorEnableCollision(bIsVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

		if (UPrimitiveComponent* WeaponMesh = Cast<UPrimitiveComponent>(Sword->GetRootComponent()))
		{
			WeaponMesh->SetSimulatePhysics(false);
		}
	}

	if (Shield)
	{
		Shield->SetActorHiddenInGame(bShouldBeHidden);
		Shield->SetActorEnableCollision(bIsVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

		if (UPrimitiveComponent* WeaponMesh = Cast<UPrimitiveComponent>(Shield->GetRootComponent()))
		{
			WeaponMesh->SetSimulatePhysics(false);
		}
	}
}

void ASLMonsterAICharacter::SpawnSpear()
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

void ASLMonsterAICharacter::SpawnArrow()
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

void ASLMonsterAICharacter::OnHitByCharacter(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                             const FHitResult& Hit)
{
	//if (OtherActor && OtherActor->IsA(ASLMonsterAICharacter::StaticClass())
	//	|| OtherActor->IsA(ASLPlayerCharacter::StaticClass()))

	if (OtherActor->IsA(ASLPlayerCharacter::StaticClass()))
	{
		if (!bRecentlyPushed)
		{
			bRecentlyPushed = true;
			FVector PushDirection = GetActorLocation() - OtherActor->GetActorLocation();
			PushDirection.Z = 0.0f;
			PushDirection.Normalize();

			LaunchCharacter(PushDirection * 300.0f, true, true);

			GetWorld()->GetTimerManager().SetTimer(PushResetHandle, this, &ASLMonsterAICharacter::ResetPushFlag, 0.5f,
			                                       false);
		}
	}
}

void ASLMonsterAICharacter::ResetPushFlag()
{
	bRecentlyPushed = false;
}

void ASLMonsterAICharacter::RotateToHitCauser(const AActor* Causer)
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

void ASLMonsterAICharacter::GetHitDirection(AActor* Causer)
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

void ASLMonsterAICharacter::AttachItemToHand(AActor* ItemActor, const FName SocketName) const
{
	if (!ItemActor || !GetMesh()) return;

	ItemActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
}

void ASLMonsterAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASLMonsterAICharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	StopFlyingState();
}

void ASLMonsterAICharacter::OnHoveredByCursor_Implementation(ASLBasePlayerController* InstigatingController)
{
	if (InstigatingController)
	{
		InstigatingController->SetEnemyCursor();
	}
}

void ASLMonsterAICharacter::OnUnhoveredByCursor_Implementation(ASLBasePlayerController* InstigatingController)
{
	if (InstigatingController)
	{
		InstigatingController->SetDefaultCursor();
	}
}

void ASLMonsterAICharacter::OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult,
                                          EHitAnimType AnimType)
{
	if (IsInPrimaryState(TAG_AI_Dead)) return;
	LastAnimType = AnimType;
	/*
	if (LastAnimType == EHitAnimType::HAT_FallBack && !IsInPrimaryState(TAG_AI_Idle))
	{
		return;
	}
	*/

	AnimationComponent->StopAllMontages(0.2f);
	AICombatComp->StopRetreating();
	AICombatComp->StartRandomTurn();
	AIStateComp->SetSingleBerserkMode(true);
	//GetBattleSoundSubSystem()->PlayBattleSound(EBattleSoundType::BST_MonsterHit, GetActorLocation());

	LastAttacker = Causer;
	CurrentHealth -= Damage;

	UE_LOG(LogTemp, Warning, TEXT("Monster Current Health[%f]"), CurrentHealth);

	if (CurrentHealth <= 0.f)
	{
		AnimationComponent->PlayAIHitMontage("Dead");
		Dead(Causer, true);
		return;
	}

	if (Causer != nullptr && Causer != this)
	{
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController != nullptr)
		{
			AIController->StopMovement();
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;

			const FVector CauserLocation = Causer->GetActorLocation();
			const FVector DirectionToCauser = (CauserLocation - GetActorLocation()).GetSafeNormal();
			const FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToCauser).Rotator();

			AIController->SetControlRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
		}
	}

	//RotateToHitCauser(Causer);
	//ChangeMeshTemporarily();
	StartFlyingState();
	if (AICombatComp)
	{
		AICombatComp->SetTarget(Causer);
	}

	switch (AnimType)
	{
	case EHitAnimType::HAT_WeakHit:
	case EHitAnimType::HAT_HardHit:
		{
			GetHitDirection(Causer);
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

			SetPrimaryState(TAG_AI_Idle);
			break;
		}
	case EHitAnimType::HAT_AirBorne:
		{
			AnimationComponent->PlayAIHitMontage("Airborne");
		}
		break;
	case EHitAnimType::HAT_AirUp:
		{
			AnimationComponent->PlayAIHitMontage("AirUp");
		}
		break;
	case EHitAnimType::HAT_FallBack:
		{
			AnimationComponent->PlayAIHitMontage("GroundHit");
		}
		break;
	case EHitAnimType::HAT_KillMotionA:
		AnimationComponent->PlayAIHitMontage("ExecutionA");
		break;
	case EHitAnimType::HAT_KillMotionB:
		AnimationComponent->PlayAIHitMontage("ExecutionB");
		break;
	case EHitAnimType::HAT_KillMotionC:
		AnimationComponent->PlayAIHitMontage("ExecutionC");
		break;
	case EHitAnimType::HAT_Parry:
		break;
	default: break;
	}
}

void ASLMonsterAICharacter::ChangeMeshTemporarily(const float Rate)
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
		&ASLMonsterAICharacter::ResetMaterial,
		Rate,
		false
	);
}

void ASLMonsterAICharacter::ResetMaterial()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	for (int32 i = 0; i < OriginalMaterials.Num(); ++i)
	{
		MeshComp->SetMaterial(i, OriginalMaterials[i]);
	}
}

void ASLMonsterAICharacter::HandleAnimNotify(EAttackAnimType MonsterMontageStage)
{
	switch (MonsterMontageStage)
	{
	case EAttackAnimType::AAT_AINormal:
	case EAttackAnimType::AAT_AISpecial:
		break;
	case EAttackAnimType::AAT_FinalAttackA:
	case EAttackAnimType::AAT_FinalAttackB:
	case EAttackAnimType::AAT_FinalAttackC:
	case EAttackAnimType::AAT_Dead:
		GetBattleSoundSubSystem()->PlayBattleSound(EBattleSoundType::BST_MonsterDie, GetActorLocation());
		break;
	case EAttackAnimType::AAT_Airborn:
		//Dead(LastAttacker, true);
		return;
	}

	StopFlyingState();
}

void ASLMonsterAICharacter::HandleHitNotify()
{
	SetPrimaryState(TAG_AI_Idle);
	StopFlyingState();
}

void ASLMonsterAICharacter::ActivateMovementComponent()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->Activate();
	}
}

void ASLMonsterAICharacter::CorrectActorLocationPostAttack()
{
	const UWorld* World = GetWorld();
	if (!World) return;

	const FVector CurrentLocation = GetActorLocation();
	const FVector Start = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z + 50.f);
	const FVector End = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - 500.f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params))
	{
		const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector CorrectedLocation = HitResult.Location + FVector(0.f, 0.f, CapsuleHalfHeight);

		SetActorLocation(CorrectedLocation, false, nullptr, ETeleportType::TeleportPhysics);
		UE_LOG(LogTemp, Warning, TEXT("%s 위치 보정 완료: %s"), *GetName(), *CorrectedLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s 위치 보정 실패: 아래에 바닥을 찾지 못했습니다."), *GetName());
	}
}

void ASLMonsterAICharacter::Dead(const AActor* Attacker, const bool bIsChangeMaterial)
{
	SetPrimaryState(TAG_AI_Dead);
	OnDeath();
	ToggleWeaponState(false);

	if (DeathMaterial && bIsChangeMaterial)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(MaterialResetTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(MaterialResetTimerHandle);
		}

		GetMesh()->SetMaterial(0, DeathMaterial);
	}

	if (ASLMonsterAIController* AICon = Cast<ASLMonsterAIController>(GetController()))
	{
		AICon->StopMovement();
		AICon->UnPossess();
		AICon->ToggleLockOnWidget(false);
	}

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

void ASLMonsterAICharacter::OnDeathMontageEnded()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HandleAIPoolReturnOnDeath();
}

void ASLMonsterAICharacter::HandleAIPoolReturnOnDeath()
{
	CurrentHealth = MaxHealth;
	OnMonsterDied.Broadcast(this);

	if (AnimationComponent)
	{
		AnimationComponent->StopAllMontages(0.0f);
	}

	if (bOriginalMaterialsInitialized)
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		for (int32 i = 0; i < OriginalMaterials.Num(); ++i)
		{
			MeshComp->SetMaterial(i, OriginalMaterials[i]);
		}
	}

	ToggleWeaponState(false);

	GetWorld()->GetTimerManager().ClearTimer(MaterialResetTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CollisionResetTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DeadTimerHandle);

	bIsHit = false;
	bRecentlyPushed = false;
	LastAttacker = nullptr;

	if (IsValid(BornSpawner))
	{
		BornSpawner->ReturnUnitToPool(this);
		UE_LOG(LogTemp, Log, TEXT("AI 유닛 '%s': 스포너 '%s'에게 풀 반납 요청 완료."), *GetName(), *BornSpawner->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI 유닛 '%s': BornSpawner를 찾을 수 없어 액터를 파괴합니다."), *GetName());
		SetLifeSpan(0.5f);
	}
}

void ASLMonsterAICharacter::FixCharacterVelocity()
{
	FVector Velocity = GetCharacterMovement()->Velocity;
	Velocity.Z = 0.f;
	GetCharacterMovement()->Velocity = Velocity;
}

void ASLMonsterAICharacter::StartFlyingState()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,
		                                                     ECollisionResponse::ECR_Ignore);
	}
}

void ASLMonsterAICharacter::StopFlyingState()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->
			SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	}
}

USLSoundSubsystem* ASLMonsterAICharacter::GetBattleSoundSubSystem() const
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

void ASLMonsterAICharacter::RecoverFromHitState()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	bIsHit = false;
}

void ASLMonsterAICharacter::PlayHitMontageAndSetupRecovery(const float Length)
{
	if (Length > 0.f)
	{
		bIsHit = true;
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		if (TimerManager.IsTimerActive(CollisionResetTimerHandle))
		{
			return;
		}

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		//GetWorld()->GetTimerManager().ClearTimer(CollisionResetTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(CollisionResetTimerHandle, this,
		                                       &ASLMonsterAICharacter::RecoverFromHitState, Length, false);
	}
	else
	{
		RecoverFromHitState();
	}
}
