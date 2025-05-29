#include "MonsterAICharacter.h"

#include "Character/BattleComponent/BattleComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controller/MonsterAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AMonsterAICharacter::AMonsterAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AMonsterAIController::StaticClass();

	AnimationComponent = CreateDefaultSubobject<UAnimationMontageComponent>(TEXT("AnimationComponent"));
	BattleComponent = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement(); MoveComp && MoveComp->MovementMode ==
		EMovementMode::MOVE_None)
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

	SetPrimaryState(TAG_AI_Idle);

	BattleComponent->OnCharacterHited.AddDynamic(this, &AMonsterAICharacter::OnHitReceived);
}

void AMonsterAICharacter::OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EAttackAnimType AnimType)
{
	AnimationComponent->StopAllMontages(0.2f);
	HitDirection(Causer);

	SetPrimaryState(TAG_AI_Hit);

	switch (AnimType)
	{
	case EAttackAnimType::AAT_NormalAttack1:
		break;
	case EAttackAnimType::AAT_NormalAttack2:
		break;
	case EAttackAnimType::AAT_NormalAttack3:
		break;
	case EAttackAnimType::AAT_SpecialAttack1:
		break;
	case EAttackAnimType::AAT_SpecialAttack2:
		break;
	case EAttackAnimType::AAT_SpecialAttack3:
		break;
	case EAttackAnimType::AAT_AirAttack1:
		break;
	case EAttackAnimType::AAT_AirAttack2:
		break;
	case EAttackAnimType::AAT_AirAttack3:
		break;
	case EAttackAnimType::AAT_Airborn:
		break;
	case EAttackAnimType::AAT_Skill1:
		break;
	case EAttackAnimType::AAT_Skill2:
		break;
	case EAttackAnimType::AAT_FinalAttackA:
		AnimationComponent->PlayAIHitMontage("ExecutionA");
		break;
	case EAttackAnimType::AAT_FinalAttackB:
		AnimationComponent->PlayAIHitMontage("ExecutionB");
		break;
	case EAttackAnimType::AAT_FinalAttackC:
		AnimationComponent->PlayAIHitMontage("ExecutionC");
		break;
	case EAttackAnimType::AAT_ParryAttack:
		break;
	default: break;
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

void AMonsterAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
		Dead(false);
		break;
	case EAttackAnimType::AAT_ParryAttack:
		break;
	default: break;
	}

	SetPrimaryState(TAG_AI_Idle);
}

void AMonsterAICharacter::Dead(bool bWithMontage)
{
	// AI 중지
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
		AICon->UnPossess();
	}

	// 애니메이션 재생 or 몽타주
	if (bWithMontage)
	{
		AnimationComponent->PlayAIHitMontage("Dead");
	}

	// 콜리전 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 나중에 Destroy 또는 사라짐 처리
	SetLifeSpan(5.f);
}
