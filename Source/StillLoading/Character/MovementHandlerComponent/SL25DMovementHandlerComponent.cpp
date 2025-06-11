#include "SL25DMovementHandlerComponent.h"

#include "SLMovementHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Character/RadarComponent/CollisionRadarComponent.h"
#include "Character/SlowMotionHelper/SlowMotionHelper.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "IO/IoChunkEncoding.h"


USL25DMovementHandlerComponent::USL25DMovementHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USL25DMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<ASLPlayerCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		OwnerCharacter->CameraBoom->bUsePawnControlRotation = false;
		OwnerCharacter->ThirdPersonCamera->bUsePawnControlRotation = false;
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		OwnerCharacter->CameraBoom->bInheritPitch = false;
		OwnerCharacter->CameraBoom->bInheritYaw = false;
		OwnerCharacter->CameraBoom->bInheritRoll = false;
		OwnerCharacter->bUseControllerRotationYaw = false;
		OwnerCharacter->CameraBoom->bEnableCameraLag = false;
		OwnerCharacter->CameraBoom->bEnableCameraRotationLag = false;

		OwnerCharacter->CameraBoom->bDoCollisionTest = false;
		
		CachedMontageComponent = OwnerCharacter->FindComponentByClass<UAnimationMontageComponent>();
		CachedCombatComponent = OwnerCharacter->FindComponentByClass<UCombatHandlerComponent>();
		CachedBattleComponent = OwnerCharacter->FindComponentByClass<UBattleComponent>();
		CachedRadarComponent = OwnerCharacter->FindComponentByClass<UCollisionRadarComponent>();
		CachedRadarComponent->OnActorDetectedEnhanced.
							  AddDynamic(this, &USL25DMovementHandlerComponent::OnRadarDetectedActor);
		CachedSkeletalMesh = OwnerCharacter->GetMesh();

		BindIMCComponent();

		APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());

		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = true;

			FInputModeGameAndUI InputModeData;
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputModeData.SetHideCursorDuringCapture(false);
            
			PlayerController->SetInputMode(InputModeData);
		}
	}
}

void USL25DMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bShouldFaceMouse)
	{
		return;
	}

	if (!OwnerCharacter || !CachedSkeletalMesh) return;
    
	APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PlayerController) return;

	FHitResult HitResult;
	if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		const FVector Direction = HitResult.Location - OwnerCharacter->GetActorLocation();
		const FVector FlattenedDirection = FVector(Direction.X, Direction.Y, 0.0f);
		const FRotator TargetRotation = FlattenedDirection.Rotation();

		const FRotator CorrectedTargetRotation = FRotator(0.0f, TargetRotation.Yaw - 90.0f, 0.0f);
		const FRotator CurrentRotation = CachedSkeletalMesh->GetRelativeRotation();
       
		const FRotator SmoothedRotation = FMath::RInterpTo(
		   CurrentRotation,
		   CorrectedTargetRotation,
		   DeltaTime,
		   RotationSpeed
		);

		CachedSkeletalMesh->SetRelativeRotation(FRotator(0.0f, SmoothedRotation.Yaw, 0.0f));
	}
}

void USL25DMovementHandlerComponent::StartFacingMouse()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_FacingBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Input Blocked"));
		return;
	}
	bShouldFaceMouse = true;
}

void USL25DMovementHandlerComponent::StopFacingMouse()
{
	bShouldFaceMouse = false;
}

void USL25DMovementHandlerComponent::OnRadarDetectedActor(AActor* DetectedActor, float Distance)
{
	
}

void USL25DMovementHandlerComponent::BindIMCComponent()
{
	if (auto* IMC = GetOwner()->FindComponentByClass<UDynamicIMCComponent>())
	{
		IMC->OnActionTriggered.AddDynamic(this, &USL25DMovementHandlerComponent::OnActionTriggered);
		IMC->OnActionStarted.AddDynamic(this, &USL25DMovementHandlerComponent::OnActionStarted);
		IMC->OnActionCompleted.AddDynamic(this, &USL25DMovementHandlerComponent::OnActionCompleted);
	}

	CachedBattleComponent->OnCharacterHited.AddDynamic(this, &USL25DMovementHandlerComponent::OnHitReceived);
}

void USL25DMovementHandlerComponent::OnActionTriggered(EInputActionType ActionType, FInputActionValue Value)
{
	//FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	//EnumName.RemoveFromStart("EInputActionType::");

	//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionTriggered → %s"), *EnumName);

	switch (ActionType)
	{
	case EInputActionType::EIAT_Look:
		break;
	case EInputActionType::EIAT_MoveUp:
	case EInputActionType::EIAT_MoveDown:
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		Move(Value.Get<float>(), ActionType);
		break;

	default:
		break;
	}
}

void USL25DMovementHandlerComponent::OnActionStarted(EInputActionType ActionType)
{
	//FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	//EnumName.RemoveFromStart("EInputActionType::");

	//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionStarted → %s"), *EnumName);

	switch (ActionType)
	{
	case EInputActionType::EIAT_Jump:
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

					OwnerCharacter->AddSecondaryState(TAG_Character_Defense_Parry);
					BlockCount = 0;
				}
				
				USlowMotionHelper::ApplyGlobalSlowMotion(OwnerCharacter, 0.2f, 0.3f);

				// 전체 슬로우 (자기 자신 포함)
				//USlowMotionHelper::QueueSlowMotionRequest(OwnerCharacter, nullptr, 0.2f, 0.15f, true, false);
				// 자기 자신 제외한 모두 슬로우
				//USlowMotionHelper::QueueSlowMotionRequest(OwnerCharacter, OwnerCharacter, 0.2f, 0.3f, true, true);

				return;
			}
			//FaceToMouse();
			StartFacingMouse();
			Attack();
			break;
		}
	case EInputActionType::EIAT_PointMove:
		break;
	case EInputActionType::EIAT_Block:
		if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
		{
			//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Input Blocked"));
			return;
		}
		//FaceToMouse();
		StartFacingMouse();
		Block(true);
		break;
	case EInputActionType::EIAT_Walk:
		break;
	case EInputActionType::EIAT_Menu:
		ToggleMenu();
	case EInputActionType::EIAT_LockObject:
		break;

	default:
		break;
	}
}

void USL25DMovementHandlerComponent::OnActionCompleted(EInputActionType ActionType)
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
		StopFacingMouse();
		Block(false);
		break;

	default:
		break;
	}
}

void USL25DMovementHandlerComponent::OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult,
                                              EAttackAnimType AnimType)
{
	if (OwnerCharacter->HasSecondaryState(TAG_Character_Defense_Parry)
		|| OwnerCharacter->IsInPrimaryState(TAG_Character_OnBuff)
		|| OwnerCharacter->HasSecondaryState(TAG_Character_Attack_Blast)) return;
	
	OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;

	bool bIsFromBack = false;
	FRotator TargetRotation;
	RotateToHitCauser(Causer, TargetRotation, bIsFromBack);

	if (OwnerCharacter->IsInPrimaryState(TAG_Character_Defense_Block) && !bIsFromBack)
	{
		if (BlockCount >= FIoChunkEncoding::MaxBlockCount && !OwnerCharacter->HasSecondaryState(TAG_Character_HitReaction_Block_Break))
		{
			OwnerCharacter->ClearAllStateTags();
			OwnerCharacter->AddSecondaryState(TAG_Character_HitReaction_Block_Break);
			CachedMontageComponent->PlayBlockMontage(FName("BlockBreak"));
			BlockCount = 0;
			LastBlockTime = 0;

			if (!GetWorld()->GetTimerManager().IsTimerActive(DelayTimerHandle))
			{
				GetWorld()->GetTimerManager().SetTimer(
					DelayTimerHandle,
					this,
					&USL25DMovementHandlerComponent::OnDelayedAction,
					5.0f,
					false
				);
			}
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

	// 피격무적
	if (OwnerCharacter->HasSecondaryState(TAG_Character_Invulnerable)) return;
	OwnerCharacter->AddSecondaryState(TAG_Character_Invulnerable);
	OwnerCharacter->GetWorldTimerManager().PauseTimer(InvulnerabilityTimerHandle);
	OwnerCharacter->GetWorldTimerManager().ClearTimer(InvulnerabilityTimerHandle);
	OwnerCharacter->GetWorldTimerManager().SetTimer(InvulnerabilityTimerHandle, this,
	                                                &USL25DMovementHandlerComponent::RemoveInvulnerability,
	                                                InvulnerableDuration, false);

	// 피격
	OwnerCharacter->ClearStateTags({}, {TAG_Character_LockOn, TAG_Character_PrepareLockOn, TAG_Character_Invulnerable, TAG_Character_Empowered});
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
	case EAttackAnimType::AAT_AISpecial:
	case EAttackAnimType::AAT_JumpAttack: // 중간거
		OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Medium);
		RemoveDelay = 1.0f;
		break;
	case EAttackAnimType::AAT_Whirlwind: // 약한거
	case EAttackAnimType::AAT_AINormal:
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

void USL25DMovementHandlerComponent::OnDelayedAction()
{
	OwnerCharacter->RemoveSecondaryState(TAG_Character_HitReaction_Block_Break);
	GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
}

void USL25DMovementHandlerComponent::RemoveInvulnerability() const
{
	OwnerCharacter->SecondaryStateTags.RemoveTag(TAG_Character_Invulnerable);
}

void USL25DMovementHandlerComponent::HitDirection(AActor* Causer)
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

void USL25DMovementHandlerComponent::RotateToHitCauser(const AActor* Causer, FRotator& TargetRotation, bool& bIsHitFromBack)
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

void USL25DMovementHandlerComponent::ToggleMenu()
{
	UE_LOG(LogTemp, Log, TEXT("Menu opened or closed"));
	// TODO: UI 호출 / Input 모드 변경 등 처리

	// HUD에 OnPose
}

void USL25DMovementHandlerComponent::Block(const bool bIsBlocking)
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

void USL25DMovementHandlerComponent::Interact()
{
	// TODO: 인터랙션 대상 탐색 및 처리
	CachedCombatComponent->SetEmpoweredCombatMode(10);
}

void USL25DMovementHandlerComponent::Attack()
{
	UAnimMontage* Montage = nullptr;
	FName SectionName;

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AttackBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Attack Blocked"));
		return;
	}

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

void USL25DMovementHandlerComponent::ApplyAttackState(const FName& SectionName, bool bIsFalling)
{
	const bool bEmpowered = CachedCombatComponent->IsEmpowered();

	if (SectionName == "Attack3")
	{
		if (bEmpowered)
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Special3);
		}
		else
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Basic3);
		}
	}
	else if (SectionName == "Attack2")
	{
		if (bEmpowered)
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
		if (bEmpowered)
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Special1);
		}
		else
		{
			OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Basic1);
		}
	}
}

void USL25DMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
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
		
		const FRotator TargetRotation(0.0f, YawRotationScalar, 0.0f);
		const FRotator CurrentRotation = CachedSkeletalMesh->GetRelativeRotation();
		const FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 10.0f);

		CachedSkeletalMesh->SetRelativeRotation(SmoothedRotation);
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
}

void USL25DMovementHandlerComponent::FaceToMouse()
{
	APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PlayerController)
	{
		return;
	}

	FHitResult HitResult;
	if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		if (CachedSkeletalMesh)
		{
			const FVector Direction = HitResult.Location - OwnerCharacter->GetActorLocation();
			const FVector FlattenedDirection = FVector(Direction.X, Direction.Y, 0.0f);
			const FRotator TargetRotation = FlattenedDirection.Rotation();

			if (CachedSkeletalMesh)
			{
				CachedSkeletalMesh->SetRelativeRotation(FRotator(0.0f, TargetRotation.Yaw, 0.0f));
			}
		}
	}
}

// 애니매이션 노티용
void USL25DMovementHandlerComponent::OnAttackStageFinished(const ECharacterMontageState AttackStage)
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
	case ECharacterMontageState::ECS_Attack_ExecutionA:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_ExecutionA);
		break;
	case ECharacterMontageState::ECS_Attack_ExecutionB:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_ExecutionB);
		break;
	case ECharacterMontageState::ECS_Attack_ExecutionC:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_ExecutionC);
		break;
	case ECharacterMontageState::ECS_Defense_Block:
		break;
	case ECharacterMontageState::ECS_Defense_Block_Break:
		break;
	case ECharacterMontageState::ECS_Defense_Parry:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Defense_Parry);
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
	case ECharacterMontageState::ECS_Attack_Begin:
		break;
	case ECharacterMontageState::ECS_Attack_BeginAir:
		break;
	default:
		break;
	}

	StopFacingMouse();
	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Idle);
}