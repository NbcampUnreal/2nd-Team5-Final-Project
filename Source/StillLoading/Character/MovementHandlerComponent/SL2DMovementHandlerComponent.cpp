#include "Character/MovementHandlerComponent/SL2DMovementHandlerComponent.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/Animation/SLAnimNotify.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "SubSystem/SLSoundTypes.h"

USL2DMovementHandlerComponent::USL2DMovementHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USL2DMovementHandlerComponent::OnAttackStageFinished(ECharacterMontageState AttackStage)
{
	switch (AttackStage)
	{
	case ECharacterMontageState::ECS_Attack_Basic1:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic1);
		break;
	case ECharacterMontageState::ECS_Attack_Basic2:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic2);
		break;
	case ECharacterMontageState::ECS_Attack_Basic3:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic3);
		break;
	case ECharacterMontageState::ECS_ETC:
		OwnerCharacter->ChangeVisibilityWeapons(true);
		break;
	default:
		break;
	}
	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Idle);
	AttackStateCount = 0;
}

void USL2DMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerCharacter)
	{
		CachedSkeletalMesh = OwnerCharacter->GetMesh();
	}

	if (auto* CMC = GetOwner()->FindComponentByClass<UCameraManagerComponent>())
	{
		constexpr EGameCameraType NextType = EGameCameraType::EGCT_TopDown;
		CMC->SwitchCamera(NextType);
	}

	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USL2DMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USL2DMovementHandlerComponent::OnActionTriggered_Implementation(EInputActionType ActionType, FInputActionValue Value)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Input Blocked"));
		return;
	}
	
	switch (ActionType)
	{
	case EInputActionType::EIAT_Look: break;
	case EInputActionType::EIAT_MoveUp:
	case EInputActionType::EIAT_MoveDown:
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		Move(Value.Get<float>(), ActionType);
		break;
	default:
		checkNoEntry();
		break;
	}
}

void USL2DMovementHandlerComponent::OnActionStarted_Implementation(EInputActionType ActionType)
{
	Super::OnActionStarted_Implementation(ActionType);
	
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Input Blocked"));
		return;
	}
	
	switch (ActionType)
	{
	case EInputActionType::EIAT_Attack:
		Attack();
		break;
	default:
		break;
	}
}

void USL2DMovementHandlerComponent::OnActionCompleted_Implementation(EInputActionType ActionType)
{
	
}

void USL2DMovementHandlerComponent::OnHitReceived_Implementation(AActor* Causer, float Damage,
	const FHitResult& HitResult, EHitAnimType AnimType)
{
	Super::OnHitReceived_Implementation(Causer, Damage, HitResult, AnimType);

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_HitBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USLMovementComponentBase: Hit Blocked"));
		return;
	}
}

void USL2DMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_MovementBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Movement Blocked"));
		return;
	}
	
	if (!OwnerCharacter || FMath::IsNearlyZero(AxisValue)) return;

	CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterGuardBreak, OwnerCharacter->GetActorLocation());

	float YawRotationScalar = 0.0f;
	switch (ActionType)
	{
		case EInputActionType::EIAT_MoveLeft:
			YawRotationScalar = 270.0f;
			break;
		case EInputActionType::EIAT_MoveRight:
			YawRotationScalar = 90.0f;
			break;
		case EInputActionType::EIAT_MoveUp:
			YawRotationScalar = 0.0f;
			break;
		case EInputActionType::EIAT_MoveDown:
			YawRotationScalar = 180.0f;
			break;
		default:
			break;
	}
	
	const FRotator NewRotation(0.0f, YawRotationScalar, 0.0f);

	const FVector ForwardDir = FVector::ForwardVector;
	const FVector RightDir = FVector::RightVector;

	OwnerCharacter->SetActorRotation(NewRotation);
	switch (ActionType)
	{
		case EInputActionType::EIAT_MoveUp:
			OwnerCharacter->AddMovementInput(ForwardDir, AxisValue);
			break;
		case EInputActionType::EIAT_MoveDown:
			OwnerCharacter->AddMovementInput(-ForwardDir, AxisValue);
			break;
		case EInputActionType::EIAT_MoveLeft:
			OwnerCharacter->AddMovementInput(-RightDir, AxisValue);
			break;
		case EInputActionType::EIAT_MoveRight:
			OwnerCharacter->AddMovementInput(RightDir, AxisValue);
			break;
		default:
			break;
	}
	
	AttackStateCount++;
	if (AttackStateCount >= AttackStateCountLimit)
	{
		CachedCombatComponent->ResetCombo();
	}
}

void USL2DMovementHandlerComponent::Attack()
{
	if (OwnerCharacter->HasSecondaryState(TAG_Character_Attack_Basic1) || OwnerCharacter->HasSecondaryState(TAG_Character_Attack_Basic3)) return;
	UAnimMontage* Montage = nullptr;
	FName SectionName;
	
	if (!CachedCombatComponent->GetCurrentComboInfo(Montage, SectionName))
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid combo found"));
		return;
	}

	const bool bIsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();
	ApplyAttackState(SectionName, bIsFalling);

	if (CachedMontageComponent && Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("SectionName[%s]"), *SectionName.ToString());
		CachedMontageComponent->PlayMontage(Montage, SectionName);
	}
	CachedCombatComponent->AdvanceCombo();

	OwnerCharacter->SetPrimaryState(TAG_Character_Attack);
}

void USL2DMovementHandlerComponent::ApplyAttackState(const FName& SectionName, bool bIsFalling)
{
	if (SectionName == "Attack3")
	{
		OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Basic3);
	}
	else if (SectionName == "Attack1")
	{
		OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Basic1);
	}
}
