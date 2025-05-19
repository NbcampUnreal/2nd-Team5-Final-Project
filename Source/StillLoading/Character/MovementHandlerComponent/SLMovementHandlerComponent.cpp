#include "SLMovementHandlerComponent.h"

#include "Character/SLPlayerCharacterBase.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/Animation/SLAnimNotify.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"

UMovementHandlerComponent::UMovementHandlerComponent(): OwnerCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLPlayerCharacter>(GetOwner());
	
	if (OwnerCharacter)
	{
		CachedMontageComponent = OwnerCharacter->FindComponentByClass<UAnimationMontageComponent>();
		CachedBattleComponent = OwnerCharacter->FindComponentByClass<UBattleComponent>();
		CachedCombatComponent = OwnerCharacter->FindComponentByClass<UCombatHandlerComponent>();

		OwnerCharacter->GetCharacterMovement()->JumpZVelocity = 500.f;
		BindIMCComponent();
	}
}

void UMovementHandlerComponent::OnActionTriggered(EInputActionType ActionType, FInputActionValue Value)
{
	//FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	//EnumName.RemoveFromStart("EInputActionType::");

	//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionTriggered → %s"), *EnumName);
	
	switch (ActionType)
	{
	case EInputActionType::EIAT_Look:
		Look(Value.Get<FVector2D>());
		break;
	case EInputActionType::EIAT_MoveUp:
	case EInputActionType::EIAT_MoveDown:
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		Move(Value.Get<float>(), ActionType);
		//Move(Value.Get<float>(), ActionType);
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::OnActionStarted(EInputActionType ActionType)
{
	//FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	//EnumName.RemoveFromStart("EInputActionType::");

	//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionStarted → %s"), *EnumName);

	switch (ActionType)
	{
	case EInputActionType::EIAT_Jump:
		Jump();
		break;
	case EInputActionType::EIAT_Interaction:
		Interact();
		break;
	case EInputActionType::EIAT_Attack:
		if (UInputBufferComponent* BufferComp = GetOwner()->FindComponentByClass<UInputBufferComponent>())
		{
			BufferComp->AddBufferedInput(ESkillType::ST_Attack);
		}
		break;
	case EInputActionType::EIAT_PointMove:
		if (UInputBufferComponent* BufferComp = GetOwner()->FindComponentByClass<UInputBufferComponent>())
		{
			BufferComp->AddBufferedInput(ESkillType::ST_PointMove);
		}
		PointMove();
		break;
	case EInputActionType::EIAT_Walk:
		ToggleWalk(true);
		break;
	case EInputActionType::EIAT_Menu:
		ToggleMenu();
		break;
	case EInputActionType::EIAT_Block:
		if (UInputBufferComponent* BufferComp = GetOwner()->FindComponentByClass<UInputBufferComponent>())
		{
			BufferComp->AddBufferedInput(ESkillType::ST_Block);
		}
		//Block(true);
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::OnActionCompleted(EInputActionType ActionType)
{
	//FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	//EnumName.RemoveFromStart("EInputActionType::");

	//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionCompleted → %s"), *EnumName);

	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:
	case EInputActionType::EIAT_MoveDown:
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		break;
	case EInputActionType::EIAT_Walk:
		ToggleWalk(false);
		break;
	case EInputActionType::EIAT_Jump:
		break;
	case EInputActionType::EIAT_Interaction:
	case EInputActionType::EIAT_Attack:
		
		break;
	case EInputActionType::EIAT_PointMove:
	case EInputActionType::EIAT_Menu:
		break;

	case EInputActionType::EIAT_Block:
		Block(false);
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::BindIMCComponent()
{
	if (auto* IMC = GetOwner()->FindComponentByClass<UDynamicIMCComponent>())
	{
		IMC->OnActionTriggered.AddDynamic(this, &UMovementHandlerComponent::OnActionTriggered);
		IMC->OnActionStarted.AddDynamic(this, &UMovementHandlerComponent::OnActionStarted);
		IMC->OnActionCompleted.AddDynamic(this, &UMovementHandlerComponent::OnActionCompleted);
	}
}

void UMovementHandlerComponent::Look(const FVector2D& Value)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_LookBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Look Blocked"));
		return;
	}
	
	if (!OwnerCharacter || Value.IsNearlyZero()) return;

	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC) return;

	FRotator ControlRot = PC->GetControlRotation();

	ControlRot.Yaw += Value.X * MouseSensitivity;
	ControlRot.Pitch += Value.Y * MouseSensitivity;
	ControlRot.Pitch = FMath::Clamp(ControlRot.Pitch, MinPitch, MaxPitch);

	ControlRot.Roll = 0.f;

	PC->SetControlRotation(ControlRot);
}

void UMovementHandlerComponent::Jump()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_JumpBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Jump Blocked"));
		return;
	}
	
	if (OwnerCharacter) OwnerCharacter->Jump();
	CachedCombatComponent->ResetCombo();

	OwnerCharacter->AddSecondaryState(TAG_Character_Movement_Jump);
}

void UMovementHandlerComponent::OnLanded(const FHitResult& Hit)
{
	OwnerCharacter->RemoveSecondaryState(TAG_Character_Movement_Jump);
	CachedCombatComponent->ResetCombo();
}

void UMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
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
}

void UMovementHandlerComponent::Interact()
{
	// TODO: 인터랙션 대상 탐색 및 처리
	if (CachedCombatComponent->IsEmpowered())
	{
		CachedCombatComponent->SetCombatMode(ECharacterComboState::CCS_Normal);
	}
	else
	{
		CachedCombatComponent->SetCombatMode(ECharacterComboState::CCS_Empowered);
	}
	
	// Test
	if (auto* CMC = GetOwner()->FindComponentByClass<UCameraManagerComponent>())
	{
		static const TArray<EGameCameraType> CameraOrder = {
			EGameCameraType::EGCT_ThirdPerson,
			EGameCameraType::EGCT_FirstPerson,
			EGameCameraType::EGCT_SideView
		};

		CurrentIndex = (CurrentIndex + 1) % CameraOrder.Num();
		const EGameCameraType NextType = CameraOrder[CurrentIndex];

		CMC->SwitchCamera(NextType);
	}
}

void UMovementHandlerComponent::Attack()
{
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
	
	if (CachedBattleComponent)
	{
		//TODO:: Data전달용으로 AI와 협의 필요
		CachedBattleComponent->PerformAttack();
	}

	OwnerCharacter->SetPrimaryState(TAG_Character_Attack);
}

void UMovementHandlerComponent::ApplyAttackState(const FName& SectionName, bool bIsFalling)
{
	if (bIsFalling)
	{
		OwnerCharacter->GetCharacterMovement()->GravityScale = 0.4f;
	}
	
	const bool bEmpowered = CachedCombatComponent->IsEmpowered();

	if (SectionName == "Attack3")
	{
		if (bIsFalling)
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Air3);
		}
		else if (bEmpowered)
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Special3);
		}
		else
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Basic3);
			CachedCombatComponent->StartCharging();
		}
	}
	else if (SectionName == "Attack2")
	{
		if (bIsFalling)
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Air2);
		}
		else if (bEmpowered)
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Special2);
		}
		else
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Basic2);
		}
	}
	else if (SectionName == "Attack1")
	{
		if (bIsFalling)
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Air1);
		}
		else if (bEmpowered)
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Special1);
		}
		else
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Basic1);
		}
	}
}

void UMovementHandlerComponent::PointMove()
{
	UE_LOG(LogTemp, Log, TEXT("PointMove triggered"));

	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC) return;

	FHitResult Hit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit && OwnerCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("PointMove: %s"), *Hit.GetActor()->GetName());
	}
}

void UMovementHandlerComponent::ToggleWalk(const bool bNewWalking)
{
	if (!OwnerCharacter) return;
	if (ASLBattlePlayerState* PS = Cast<ASLBattlePlayerState>(OwnerCharacter->GetPlayerState()))
	{
		PS->SetWalking(bNewWalking);
	}
}

void UMovementHandlerComponent::ToggleMenu()
{
	UE_LOG(LogTemp, Log, TEXT("Menu opened or closed"));
	// TODO: UI 호출 / Input 모드 변경 등 처리
	
}

void UMovementHandlerComponent::Block(const bool bIsBlocking)
{
	if (bIsBlocking)
	{
		OwnerCharacter->SetPrimaryState(TAG_Character_Defense_Block);
	}
	else
	{
		OwnerCharacter->RemovePrimaryState(TAG_Character_Defense_Block);
	}
}



// 애니매이션 노티용
void UMovementHandlerComponent::OnAttackStageFinished(ECharacterMontageState AttackStage)
{
	if (!OwnerCharacter) return;
	
	switch (AttackStage)
	{
	case ECharacterMontageState::ECS_Idle:
		break;
	case ECharacterMontageState::ECS_Falling:
		break;
	case ECharacterMontageState::ECS_Dodging:
		break;
	case ECharacterMontageState::ECS_Dead:
		break;
	case ECharacterMontageState::ECS_InputLocked:
		break;
	case ECharacterMontageState::ECS_Hit_Weak:
		break;
	case ECharacterMontageState::ECS_Hit_Medium:
		break;
	case ECharacterMontageState::ECS_Hit_Area:
		break;
	case ECharacterMontageState::ECS_Hit_Airborne:
		break;
	case ECharacterMontageState::ECS_Hit_Groggy:
		break;
	case ECharacterMontageState::ECS_Hit_Knockback:
		break;
	case ECharacterMontageState::ECS_Attack_Basic1:
		if (CachedMontageComponent)
		{
			CachedMontageComponent->StopAttackMontage();
		}
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic1);
		break;
	case ECharacterMontageState::ECS_Attack_Basic2:
		if (CachedMontageComponent)
		{
			CachedMontageComponent->StopAttackMontage();
		}
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic2);
		break;
	case ECharacterMontageState::ECS_Attack_Basic3:
		if (CachedMontageComponent)
		{
			CachedMontageComponent->StopAttackMontage();
		}
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic3);
		break;
	case ECharacterMontageState::ECS_Attack_Special1:
		if (CachedMontageComponent)
		{
			CachedMontageComponent->StopAttackMontage();
		}
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Special1);
		break;
	case ECharacterMontageState::ECS_Attack_Special2:
		if (CachedMontageComponent)
		{
			CachedMontageComponent->StopAttackMontage();
		}
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Special2);
		break;
	case ECharacterMontageState::ECS_Attack_Special3:
		if (CachedMontageComponent)
		{
			CachedMontageComponent->StopAttackMontage();
		}
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Special3);
		break;
	case ECharacterMontageState::ECS_Attack_Air1:
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Air1);
		break;
	case ECharacterMontageState::ECS_Attack_Air2:
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Air2);
		break;
	case ECharacterMontageState::ECS_Attack_Air3:
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
		OwnerCharacter->RemovePrimaryState(TAG_Character_Attack);
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Air3);
		break;
	case ECharacterMontageState::ECS_Attack_Airborn1:
	case ECharacterMontageState::ECS_Attack_Finisher1:
	case ECharacterMontageState::ECS_Attack_Finisher2:
		break;
	case ECharacterMontageState::ECS_Defense_Block:
		break;
	case ECharacterMontageState::ECS_Defense_Parry:
		break;
	case ECharacterMontageState::ECS_Cinematic:
		break;
	case ECharacterMontageState::ECS_Moving:
		break;
	case ECharacterMontageState::ECS_Jumping:
		break;
	case ECharacterMontageState::ECS_Charge_Basic:
		break;
	case ECharacterMontageState::ECS_Charge_Special:
		break;
	default:
		break;
	}
}

// 버퍼 수행용
void UMovementHandlerComponent::HandleBufferedInput(ESkillType Action)
{
	switch (Action)
	{
	case ESkillType::ST_Attack:
		Attack();
		break;
	case ESkillType::ST_Block:
		Block(true);
		break;
	default:
		break;
	}
}