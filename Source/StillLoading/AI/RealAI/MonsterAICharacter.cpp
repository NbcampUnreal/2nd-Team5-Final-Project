#include "MonsterAICharacter.h"

#include "FormationComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controller/MonsterAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AMonsterAICharacter::AMonsterAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 10.0f;
	CurrentHealth = MaxHealth;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AMonsterAIController::StaticClass();

	AnimationComponent = CreateDefaultSubobject<UAnimationMontageComponent>(TEXT("AnimationComponent"));
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
	FormationComponent = CreateDefaultSubobject<UFormationComponent>(TEXT("FormationComponent"));

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMonsterAICharacter::OnHitByCharacter);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement();
		MoveComp && MoveComp->MovementMode == EMovementMode::MOVE_None)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
}

void AMonsterAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsLeader)
	{
		FVector Location = GetActorLocation();

		// 머리 위에 텍스트 표시
		FVector TextLocation = Location + FVector(0, 0, 120);
		DrawDebugString(
			GetWorld(),
			TextLocation,
			TEXT("Leader"),
			nullptr,
			FColor::White,
			0.f, // 지속 시간
			true // 카메라 고정 여부
		);
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

	SetPrimaryState(TAG_AI_Idle);
	SetStrategyState(TAG_AI_STRATEGY_SINGLE_SNEAKY);

	BattleComponent->OnCharacterHited.AddDynamic(this, &AMonsterAICharacter::OnHitReceived);

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 150.f;
}

void AMonsterAICharacter::OnHitByCharacter(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(AMonsterAICharacter::StaticClass()) || OtherActor->IsA(ASLPlayerCharacter::StaticClass()))
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
		}
	}
}

void AMonsterAICharacter::ResetPushFlag()
{
	bRecentlyPushed = false;
}

void AMonsterAICharacter::SetLeader()
{
	bIsLeader = true;

	AAIController* LeaderController = Cast<AAIController>(GetController());

	if (LeaderController && LeaderController->GetBlackboardComponent())
	{
		LeaderController->GetBlackboardComponent()->SetValueAsObject(FName("Leader"), this);
		LeaderController->GetBlackboardComponent()->SetValueAsVector(FName("LeaderOrderLocation"), FVector::ZeroVector);
	}
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

	if (Dot >= -0.7f) // 뒤쪽 아님
	{
	}
	else
	{
	}
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

void AMonsterAICharacter::SetChasing(bool bEnable)
{
	bIsChasing = bEnable;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = bEnable ? 400.f : 400.f;
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
	StrategyStateTags.AddTag(NewState);
}

bool AMonsterAICharacter::HasStrategyState(const FGameplayTag StateToCheck) const
{
	return StrategyStateTags.HasTag(StateToCheck);
}

void AMonsterAICharacter::OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult,
                                        EAttackAnimType AnimType)
{
	AnimationComponent->StopAllMontages(0.2f);
	HitDirection(Causer);

	LastAttacker = Causer;
	CurrentHealth -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth [%f]"), CurrentHealth);

	switch (AnimType)
	{
	case EAttackAnimType::AAT_NormalAttack1:
	case EAttackAnimType::AAT_NormalAttack2:
	case EAttackAnimType::AAT_NormalAttack3:
	case EAttackAnimType::AAT_SpecialAttack1:
	case EAttackAnimType::AAT_SpecialAttack2:
	case EAttackAnimType::AAT_SpecialAttack3:
	case EAttackAnimType::AAT_AirAttack1:
	case EAttackAnimType::AAT_AirAttack2:
	case EAttackAnimType::AAT_AirAttack3:
		if (GetCharacterMovement()->IsFalling())
		{
			AnimationComponent->PlayAIHitMontage("HitAir");
			FVector Velocity = GetCharacterMovement()->Velocity;
			Velocity.Z = 0.f;
			GetCharacterMovement()->Velocity = Velocity;
		}
		SetBattleState(TAG_AI_Hit);

		FVector PushDirection = GetActorLocation() - Causer->GetActorLocation();
		PushDirection.Z = 0.0f;
		PushDirection.Normalize();

		LaunchCharacter(PushDirection * 500.0f, true, true);

		if (CurrentHealth < 0.f)
		{
			AnimationComponent->PlayAIHitMontage("Dead");
		}
		break;

	case EAttackAnimType::AAT_Airborn:
		AnimationComponent->PlayAIHitMontage("Airborne");
		SetBattleState(TAG_AI_Hit);
		if (CurrentHealth < 0.f)
		{
			Dead(Causer);
		}
		break;
	case EAttackAnimType::AAT_Skill1:
		AnimationComponent->PlayAIHitMontage("AirUp");
		SetBattleState(TAG_AI_Hit);
		if (CurrentHealth < 0.f)
		{
			Dead(Causer);
		}
		break;
	case EAttackAnimType::AAT_Skill2:
		AnimationComponent->PlayAIHitMontage("GroundHit");
		RotateToHitCauser(Causer);
		SetBattleState(TAG_AI_Hit);
		if (CurrentHealth < 0.f)
		{
			Dead(Causer);
		}
		break;
	case EAttackAnimType::AAT_FinalAttackA:
		AnimationComponent->PlayAIHitMontage("ExecutionA");
		SetBattleState(TAG_AI_Dead);
		break;
	case EAttackAnimType::AAT_FinalAttackB:
		AnimationComponent->PlayAIHitMontage("ExecutionB");
		SetBattleState(TAG_AI_Dead);
		break;
	case EAttackAnimType::AAT_FinalAttackC:
		AnimationComponent->PlayAIHitMontage("ExecutionC");
		SetBattleState(TAG_AI_Dead);
		break;
	case EAttackAnimType::AAT_ParryAttack:
		break;
	default: break;
	}
}

void AMonsterAICharacter::HandleAnimNotify(EAttackAnimType MonsterMontageStage)
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
		Dead(LastAttacker);
		break;
	case EAttackAnimType::AAT_ParryAttack:
		break;
	default: break;
	}

	SetBattleState(TAG_AI_Idle);
}

void AMonsterAICharacter::Dead(const AActor* Attacker)
{
	SetBattleState(TAG_AI_Dead);

	// AI 중지
	if (AMonsterAIController* AICon = Cast<AMonsterAIController>(GetController()))
	{
		AICon->StopMovement();
		AICon->UnPossess();
		AICon->ToggleLockOnWidget(false);
	}

	// 콜리전 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 나중에 Destroy 또는 사라짐 처리
	SetLifeSpan(1.f);

	// BattleComponent에 전달
	if (Attacker)
	{
		if (UBattleComponent* AttackerBattleComp = Attacker->FindComponentByClass<UBattleComponent>())
		{
			IEnemyDeathReceiver::Execute_OnEnemyDeath(AttackerBattleComp, this);
		}
	}
}
