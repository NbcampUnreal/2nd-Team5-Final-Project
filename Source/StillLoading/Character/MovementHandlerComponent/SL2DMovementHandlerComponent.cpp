
// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementHandlerComponent/SL2DMovementHandlerComponent.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/Animation/SLAnimNotify.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/Interaction/SLInteractionComponent.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interactable/SLInteractableObjectBase.h"

USL2DMovementHandlerComponent::USL2DMovementHandlerComponent(): OwnerCharacter(nullptr), CachedSkeletalMesh(nullptr)
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
	default:
		break;
	}
	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Idle);
	AttackStateCount = 0;
}

void USL2DMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLPlayerCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		CachedMontageComponent = OwnerCharacter->FindComponentByClass<UAnimationMontageComponent>();
		CachedBattleComponent = OwnerCharacter->FindComponentByClass<UBattleComponent>();
		CachedCombatComponent = OwnerCharacter->FindComponentByClass<UCombatHandlerComponent>();
		CachedInteractionComponent = OwnerCharacter->FindComponentByClass<USLInteractionComponent>();
		CachedSkeletalMesh = OwnerCharacter->GetMesh();

		BindIMCComponent();
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

void USL2DMovementHandlerComponent::BindIMCComponent()
{
	if (auto* IMC = GetOwner()->FindComponentByClass<UDynamicIMCComponent>())
	{
		IMC->OnActionTriggered.AddDynamic(this, &ThisClass::OnActionTriggered);
		IMC->OnActionStarted.AddDynamic(this, &ThisClass::OnActionStarted);
		IMC->OnActionCompleted.AddDynamic(this, &ThisClass::OnActionCompleted);
	}
}

void USL2DMovementHandlerComponent::OnActionTriggered(EInputActionType ActionType, FInputActionValue Value)
{
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

void USL2DMovementHandlerComponent::OnActionStarted(EInputActionType ActionType)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_Attack:
		Attack();
		break;
	case EInputActionType::EIAT_Interaction:
		Interaction();
		break;
	default:
		break;
	}
}

void USL2DMovementHandlerComponent::OnActionCompleted(EInputActionType ActionType)
{
	
}

void USL2DMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_MovementBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Movement Blocked"));
		return;
	}
	
	if (!OwnerCharacter || FMath::IsNearlyZero(AxisValue)) return;
	
	AController* Controller = OwnerCharacter->GetController();
	if (!Controller) return;
	
	FRotator ControlRotation = Controller->GetControlRotation();
	FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	
	FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	ForwardDir.Z = 0.f;
	RightDir.Z = 0.f;
	ForwardDir.Normalize();
	RightDir.Normalize();
	
	// Rotate mesh based on movement input
	if (CachedSkeletalMesh)
	{
		float YawRotationScalar = 0.0f;
		switch (ActionType)
		{
			case EInputActionType::EIAT_MoveLeft:
				YawRotationScalar = 180.0f;
				break;
			case EInputActionType::EIAT_MoveRight:
				YawRotationScalar = 0.0f;
				break;
			case EInputActionType::EIAT_MoveUp:
				YawRotationScalar = -90.0f;
				break;
			case EInputActionType::EIAT_MoveDown:
				YawRotationScalar = 90.0f;
				break;
			default:
				break;
		}
		const FRotator NewRotation(0.0f, YawRotationScalar, 0.0f);
			CachedSkeletalMesh->SetRelativeRotation(NewRotation);
	}

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

void USL2DMovementHandlerComponent::Interaction()
{
	if (ASLInteractableObjectBase* InteractableObject = CachedInteractionComponent->GetInteractableObject())
	{
		if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(GetOwner()))
		{
			InteractableObject->TriggerReact(PlayerCharacter, ESLReactiveTriggerType::ERT_InteractKey);
		}
	}
}
