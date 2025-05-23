#include "SLMovementHandlerComponent.h"

#include "Character/SLPlayerCharacterBase.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/Animation/SLAnimNotify.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"

UMovementHandlerComponent::UMovementHandlerComponent(): OwnerCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLPlayerCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		CachedMontageComponent = OwnerCharacter->FindComponentByClass<UAnimationMontageComponent>();
		CachedCombatComponent = OwnerCharacter->FindComponentByClass<UCombatHandlerComponent>();
		CachedBattleComponent = OwnerCharacter->FindComponentByClass<UBattleComponent>();

		OwnerCharacter->GetCharacterMovement()->JumpZVelocity = 600.f;
		BindIMCComponent();
	}
}

void UMovementHandlerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDoKnockback && OwnerCharacter)
	{
		KnockbackTimer += DeltaTime;

		FVector DeltaMove = -OwnerCharacter->GetActorForwardVector() * KnockbackSpeed * DeltaTime;
		OwnerCharacter->SetActorLocation(OwnerCharacter->GetActorLocation() + DeltaMove, true);

		if (KnockbackTimer >= KnockbackTime)
		{
			bDoKnockback = false;
		}
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
		// 패링 진입
		{
			const float CurrentTime = GetWorld()->GetTimeSeconds();
			if (CurrentTime - LastBlockTime <= ParryDuration)
			{
				UE_LOG(LogTemp, Warning, TEXT("Parring!!!"));

				if (!CachedCombatComponent->IsEmpowered())
				{
					OwnerCharacter->ClearAllStateTags();

					if (RightDot > 0.0f)
						CachedMontageComponent->PlayBlockMontage(FName("ParryR"));
					else
						CachedMontageComponent->PlayBlockMontage(FName("ParryL"));

					OwnerCharacter->SetPrimaryState(TAG_Character_Defense_Parry);
					BlockCount = 0;
				}

				CachedCombatComponent->SetEmpoweredCombatMode(ECharacterComboState::CCS_Empowered);
				return;
			}
		}
	case EInputActionType::EIAT_PointMove:
	case EInputActionType::EIAT_Block:
		if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
		{
			//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Input Blocked"));
			return;
		}
		if (UInputBufferComponent* BufferComp = GetOwner()->FindComponentByClass<UInputBufferComponent>())
		{
			BufferComp->OnIMCActionStarted(ActionType);
		}
		break;
	case EInputActionType::EIAT_Walk:
		ToggleWalk(true);
		break;
	case EInputActionType::EIAT_Menu:
		ToggleMenu();
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

	CachedBattleComponent->OnCharacterHited.AddDynamic(this, &UMovementHandlerComponent::OnHitReceived);
}

void UMovementHandlerComponent::OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult,
                                              EAttackAnimType AnimType)
{
	if (OwnerCharacter->IsInPrimaryState(TAG_Character_Defense_Parry)) return;

	bool bIsFromBack = false;
	FRotator TargetRotation;
	RotateToHitCauser(Causer, TargetRotation, bIsFromBack);

	if (OwnerCharacter->IsInPrimaryState(TAG_Character_Defense_Block) && !bIsFromBack)
	{
		if (BlockCount >= 2)
		{
			OwnerCharacter->ClearAllStateTags();
			OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Block_Break);
			CachedMontageComponent->PlayBlockMontage(FName("BlockBreak"));
			BlockCount = 0;
			LastBlockTime = 0;
		}
		else
		{
			// Parry
			HitDirection(Causer);
			const float CurrentTime = GetWorld()->GetTimeSeconds();
			LastBlockTime = CurrentTime;

			CachedMontageComponent->PlayBlockMontage(FName("BlockHit"));
			++BlockCount;
		}
		return;
	}

	OwnerCharacter->ClearAllStateTags();
	CachedMontageComponent->StopAllMontages(0.2f);

	float RemoveDelay = 1.0f;

	switch (AnimType)
	{
	case EAttackAnimType::AAT_Attack_01:
	case EAttackAnimType::AAT_Attack_02:
	case EAttackAnimType::AAT_Attack_04:
	case EAttackAnimType::AAT_DashAttack:
	case EAttackAnimType::AAT_ThrowStone: // 날라가는거
		if (OwnerCharacter->GetCharacterMovement()->IsFalling())
		{
			OwnerCharacter->SetActorRotation(TargetRotation);
			CachedMontageComponent->PlayHitMontage(FName("Fall"));
			OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Falling);
		}
		else
		{
			OwnerCharacter->SetActorRotation(TargetRotation);
			if (bIsFromBack)
			{
				CachedMontageComponent->PlayHitMontage(FName("KnockBack_Back"));
			}
			else
			{
				CachedMontageComponent->PlayHitMontage(FName("KnockBack"));
			}
			OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Knockback);
		}
		return;
	case EAttackAnimType::AAT_Attack_03: // 기절
		if (OwnerCharacter->GetCharacterMovement()->IsFalling())
		{
			OwnerCharacter->SetActorRotation(TargetRotation);
			CachedMontageComponent->PlayHitMontage(FName("Fall"));
			OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Falling);
		}
		else
		{
			OwnerCharacter->SetActorRotation(TargetRotation);
			if (bIsFromBack)
			{
				CachedMontageComponent->PlayHitMontage(FName("Groggy_Back"));
			}
			else
			{
				CachedMontageComponent->PlayHitMontage(FName("Groggy"));
			}
			OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Groggy);
		}
		return;
	case EAttackAnimType::AAT_FootAttack_Left:
	case EAttackAnimType::AAT_FootAttack_Right:
	case EAttackAnimType::AAT_GroundSlam_01:
	case EAttackAnimType::AAT_GroundSlam_02:
	case EAttackAnimType::AAT_JumpAttack: // 중간거
		OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Medium);
		RemoveDelay = 1.0f;
		break;
	case EAttackAnimType::AAT_Whirlwind: // 약한거
		OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Weak);
		RemoveDelay = 1.0f;
		break;
	default: break;
	}

	HitDirection(Causer);

	GetWorld()->GetTimerManager().SetTimer(
		ReactionResetTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if (OwnerCharacter)
			{
				OwnerCharacter->RemovePrimaryState(TAG_Character_HitReaction_Medium);
				OwnerCharacter->RemovePrimaryState(TAG_Character_HitReaction_Weak);
			}
		}),
		RemoveDelay, false
	);
}

void UMovementHandlerComponent::HitDirection(AActor* Causer)
{
	if (!OwnerCharacter || !Causer) return;

	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector CauserLocation = Causer->GetActorLocation();

	const FVector ToCauser = (CauserLocation - OwnerLocation).GetSafeNormal2D();
	const FVector Forward = OwnerCharacter->GetActorForwardVector().GetSafeNormal2D();
	const FVector Right = OwnerCharacter->GetActorRightVector().GetSafeNormal2D();

	ForwardDot = FVector::DotProduct(Forward, ToCauser);
	RightDot = FVector::DotProduct(Right, ToCauser);
}

void UMovementHandlerComponent::RotateToHitCauser(const AActor* Causer, FRotator& TargetRotation, bool& bIsHitFromBack)
{
	if (!OwnerCharacter || !Causer) return;

	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector CauserLocation = Causer->GetActorLocation();

	const FVector ToCauser = (CauserLocation - OwnerLocation).GetSafeNormal2D();
	const FVector Forward = OwnerCharacter->GetActorForwardVector().GetSafeNormal2D();

	const float Dot = FVector::DotProduct(Forward, ToCauser);

	if (Dot >= -0.7f) // 뒤쪽이 아니면 회전
	{
		TargetRotation = ToCauser.Rotation();
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;

		bIsHitFromBack = false;
	}
	else
	{
		bIsHitFromBack = true;
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

	if (OwnerCharacter)
	{
		OwnerCharacter->Jump();
	}

	CachedCombatComponent->ResetCombo();
	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Jump);
}

void UMovementHandlerComponent::OnLanded(const FHitResult& Hit)
{
	OwnerCharacter->RemovePrimaryState(TAG_Character_Movement_Jump);
	CachedCombatComponent->ResetCombo();
}

void UMovementHandlerComponent::StartKnockback(float Speed, float Duration)
{
	KnockbackSpeed = Speed;
	KnockbackTime = Duration;
	KnockbackTimer = 0.f;
	bDoKnockback = true;
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

	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:
		if (Controller && AxisValue != 0.0f)
		{
			const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			OwnerCharacter->AddMovementInput(Direction, AxisValue);
		}
		break;
	case EInputActionType::EIAT_MoveDown:
		if (Controller && AxisValue != 0.0f)
		{
			const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			OwnerCharacter->AddMovementInput(-Direction, AxisValue);
		}
		break;
	case EInputActionType::EIAT_MoveLeft:
		if (Controller && AxisValue != 0.0f)
		{
			const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			OwnerCharacter->AddMovementInput(-Direction, AxisValue);
		}
		break;
	case EInputActionType::EIAT_MoveRight:
		if (Controller && AxisValue != 0.0f)
		{
			const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			OwnerCharacter->AddMovementInput(Direction, AxisValue);
		}
		break;
	default:
		break;
	}
}

void UMovementHandlerComponent::Interact()
{
	// TODO: 인터랙션 대상 탐색 및 처리
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

void UMovementHandlerComponent::Dodge()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AirBlock) || OwnerCharacter->GetMovementComponent()->
		IsFalling())
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Dodge Blocked"));
		return;
	}
	CachedMontageComponent->PlaySkillMontage(FName("Dodge"));
	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Dodge);
}

void UMovementHandlerComponent::Airborne()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AirBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Dodge Blocked"));
		return;
	}
	CachedMontageComponent->PlaySkillMontage(FName("Airborne"));

	OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Airborne);
	OwnerCharacter->SetPrimaryState(TAG_Character_Attack);
}

void UMovementHandlerComponent::AirUp()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AirBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Dodge Blocked"));
		return;
	}
	CachedMontageComponent->PlaySkillMontage(FName("AirUp"));

	OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Airup);
	OwnerCharacter->SetPrimaryState(TAG_Character_Attack);
}

void UMovementHandlerComponent::AirDown()
{
	CachedMontageComponent->PlaySkillMontage(FName("AirDown"));

	OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Airdown);
	OwnerCharacter->SetPrimaryState(TAG_Character_Attack);
}

void UMovementHandlerComponent::Block(const bool bIsBlocking)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_DefenceBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Defence Blocked"));
		return;
	}

	if (bIsBlocking && !OwnerCharacter->GetCharacterMovement()->IsFalling())
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
	case ECharacterMontageState::ECS_Hit_Falling:
		break;
	case ECharacterMontageState::ECS_Hit_Groggy:
		break;
	case ECharacterMontageState::ECS_Hit_Knockback:
		break;
	case ECharacterMontageState::ECS_Attack_Basic1:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic1);
		break;
	case ECharacterMontageState::ECS_Attack_Basic2:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic2);
		break;
	case ECharacterMontageState::ECS_Attack_Basic3:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Basic3);
		break;
	case ECharacterMontageState::ECS_Attack_Special1:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Special1);
		break;
	case ECharacterMontageState::ECS_Attack_Special2:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Special2);
		break;
	case ECharacterMontageState::ECS_Attack_Special3:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Special3);
		break;
	case ECharacterMontageState::ECS_Attack_Air1:
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Air1);
		break;
	case ECharacterMontageState::ECS_Attack_Air2:
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Air2);
		break;
	case ECharacterMontageState::ECS_Attack_Air3:
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Air3);
		break;
	case ECharacterMontageState::ECS_Attack_Airborne:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Airborne);
		break;
	case ECharacterMontageState::ECS_Attack_Airup:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Airup);
		break;
	case ECharacterMontageState::ECS_Attack_Airdown:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Airdown);
		break;
	case ECharacterMontageState::ECS_Defense_Block:
		break;
	case ECharacterMontageState::ECS_Defense_Block_Break:
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

	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Idle);
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
	case ESkillType::ST_Dodge:
		Dodge();
		break;
	case ESkillType::ST_Airborne:
		Airborne();
		break;
	case ESkillType::ST_AirUp:
		AirUp();
		break;
	case ESkillType::ST_Airdown:
		AirDown();
		break;
	case ESkillType::ST_PointMove:
		PointMove();
		break;
	default:
		break;
	}
}
