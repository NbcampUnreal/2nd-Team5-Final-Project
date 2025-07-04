#include "SLMovementHandlerComponent.h"

#include "MotionWarpingComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "AI/RealAI/Controller/MonsterAIController.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/Animation/SLAnimNotify.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Character/RadarComponent/CollisionRadarComponent.h"
#include "Character/Skill/SLSkillComponent.h"
#include "Character/SlowMotionHelper/SlowMotionHelper.h"
#include "Controller/SLBasePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SubSystem/SLSoundSubsystem.h"

UMovementHandlerComponent::UMovementHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerCharacter && OwnerCharacter->CameraBoom)
	{
		DefaultArmLength = OwnerCharacter->CameraBoom->TargetArmLength;
		DesiredArmLength = DefaultArmLength;

		OwnerCharacter->GetCharacterMovement()->JumpZVelocity = 500.f;
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 700.0f;
		OwnerCharacter->GetCharacterMovement()->MaxAcceleration = 8192.0f;

		DefaultGravityScale = OwnerCharacter->GetCharacterMovement()->GravityScale;
		DefaultBrakingDecelerationFalling = OwnerCharacter->GetCharacterMovement()->BrakingDecelerationFalling;

		SetViewMode(false);
	}

	if (CachedRadarComponent)
	{
		CachedRadarComponent->ToggleRadarComponent(true);
		CachedRadarComponent->OnActorDetectedEnhanced.
		                      AddDynamic(this, &UMovementHandlerComponent::OnRadarDetectedActor);
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

	// Lock On
	if (OwnerCharacter->HasSecondaryState(TAG_Character_LockOn))
	{
		if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_LockOnBlock))
		{
			//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Lock On Blocked"));
			return;
		}
		
		if (CameraFocusTarget)
		{
			float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), CameraFocusTarget->GetActorLocation());
			if (Distance > FocusMaxDistance)
			{
				DisableLock();
				return;
			}

			RotateCameraToTarget(CameraFocusTarget, DeltaTime);
		}
	}

	if (OwnerCharacter && OwnerCharacter->CameraBoom)
	{
		if (FMath::IsNearlyEqual(OwnerCharacter->CameraBoom->TargetArmLength, DesiredArmLength))
		{
			return;
		}

		OwnerCharacter->CameraBoom->TargetArmLength = FMath::FInterpTo(
			OwnerCharacter->CameraBoom->TargetArmLength, // 현재 값
			DesiredArmLength, // 목표 값
			DeltaTime, // 프레임 시간
			CameraZoomInterpSpeed // 보간 속도
		);
	}
}

void UMovementHandlerComponent::OnActionTriggered_Implementation(EInputActionType ActionType, FInputActionValue Value)
{
	//FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	//EnumName.RemoveFromStart("EInputActionType::");

	//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionTriggered → %s"), *EnumName);

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Input Blocked"));
		return;
	}

	switch (ActionType)
	{
	case EInputActionType::EIAT_Look:
		Look(Value.Get<FVector2D>());
		break;
	case EInputActionType::EIAT_CameraUnlock:
		ToggleCameraState(true);
		break;
	case EInputActionType::EIAT_MoveUp:
	case EInputActionType::EIAT_MoveDown:
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		if (bActivateTPSView)
		{
			MoveTPS(Value.Get<float>(), ActionType);
		}
		else
		{
			Move(Value.Get<float>(), ActionType);
		}
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::OnActionStarted_Implementation(EInputActionType ActionType)
{
	Super::OnActionStarted_Implementation(ActionType);
	//FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	//EnumName.RemoveFromStart("EInputActionType::");

	//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionStarted → %s"), *EnumName);

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Input Blocked"));
		return;
	}

	switch (ActionType)
	{
	case EInputActionType::EIAT_Jump:
		Jump();
		break;
	case EInputActionType::EIAT_Attack:
		if (ParryCheck()) break;
	case EInputActionType::EIAT_PointMove:
	case EInputActionType::EIAT_Block:
		if (UInputBufferComponent* BufferComp = GetOwner()->FindComponentByClass<UInputBufferComponent>())
		{
			BufferComp->OnIMCActionStarted(ActionType);
		}
		break;
	case EInputActionType::EIAT_Interaction:
		break;
	case EInputActionType::EIAT_Walk:
		DodgeLoco();
		break;
	case EInputActionType::EIAT_LockObject:
		ToggleLockState();
		break;
	case EInputActionType::EIAT_ChangeView:
		SetViewMode(!bActivateTPSView);
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::OnActionCompleted_Implementation(EInputActionType ActionType)
{
	//FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	//EnumName.RemoveFromStart("EInputActionType::");

	//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionCompleted → %s"), *EnumName);

	switch (ActionType)
	{
	case EInputActionType::EIAT_CameraUnlock:
		ToggleCameraState(false);
		break;
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
		Block(false);
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::OnRadarDetectedActor(AActor* DetectedActor, float Distance)
{
	if (!OwnerCharacter->HasSecondaryState(TAG_Character_PrepareLockOn)) return;

	if (DetectedActor->IsA(AMonsterAICharacter::StaticClass())
		|| DetectedActor->IsA(ASLAIBaseCharacter::StaticClass()))
	{
		if (const ASLAIBaseCharacter* DetectedActorTemp = Cast<ASLAIBaseCharacter>(DetectedActor))
		{
			if (const IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(
				DetectedActorTemp->GetController()))
			{
				if (const ASLBasePlayerController* PlayerController = Cast<ASLBasePlayerController>(
					OwnerCharacter->GetController()))
				{
					if (TeamAgentInterface->GetGenericTeamId() == PlayerController->GetGenericTeamId()) return;
				}
			}
		}

		if (Distance <= FocusMaxDistance && IsValid(DetectedActor))
		{
			if (OwnerCharacter->HasSecondaryState(TAG_Character_LockOn)) return;
			CameraFocusTarget = DetectedActor;
			OwnerCharacter->EnableLockOnMode();

			if (const APawn* Pawn = Cast<APawn>(DetectedActor))
			{
				if (AMonsterAIController* AIController = Cast<AMonsterAIController>(Pawn->GetController()))
				{
					AIController->ToggleLockOnWidget(true);
				}
			}
		}

		/*
		const FVector Start = OwnerCharacter->GetActorLocation();
		const FVector End = DetectedActor->GetActorLocation();

		FHitResult HitResult;
		const FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(RadarLineTrace), true, OwnerCharacter);

		const bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECC_Visibility,
			TraceParams
		);

		if (bHit)
		{
			if (OwnerCharacter->HasSecondaryState(TAG_Character_LockOn)) return;
			
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.5f, 0, 1.0f);
			AActor* HitActor = HitResult.GetActor();
			CameraFocusTarget = HitActor;
			OwnerCharacter->EnableLockOnMode();
		}
		*/
	}
}

void UMovementHandlerComponent::FixCharacterVelocity()
{
	/*
	FVector Velocity = OwnerCharacter->GetCharacterMovement()->Velocity;
	Velocity.Z = 0.f;
	OwnerCharacter->GetCharacterMovement()->Velocity = Velocity;
	OwnerCharacter->GetCharacterMovement()->GravityScale = 0.f;
	*/

	if (!OwnerCharacter) return;
    
	UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	MoveComp->BrakingDecelerationFalling = 10.f;
	MoveComp->GravityScale = 0.1f;
	
	FVector CurrentVelocity = MoveComp->Velocity;
	CurrentVelocity.Z = 0;
	MoveComp->Velocity = CurrentVelocity;
}

void UMovementHandlerComponent::RemoveInvulnerability() const
{
	OwnerCharacter->SecondaryStateTags.RemoveTag(TAG_Character_Invulnerable);
}

void UMovementHandlerComponent::OnHitReceived_Implementation(AActor* Causer, float Damage, const FHitResult& HitResult,
                                                             EHitAnimType AnimType)
{
	Super::OnHitReceived_Implementation(Causer, Damage, HitResult, AnimType);

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_HitBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USLMovementComponentBase: Hit Blocked"));
		return;
	}

	if (OwnerCharacter->HasSecondaryState(TAG_Character_Defense_Parry)
		|| OwnerCharacter->IsInPrimaryState(TAG_Character_OnBuff)
		|| OwnerCharacter->HasSecondaryState(TAG_Character_Attack_Blast)
		|| OwnerCharacter->IsInPrimaryState(TAG_Character_Attack_SpawnSword))
		return;

	OwnerCharacter->GetCharacterMovement()->GravityScale = 1.0f;

	bool bIsFromBack = false;
	FRotator TargetRotation;
	RotateToHitCauser(Causer, TargetRotation, bIsFromBack);

	if (CachedCombatComponent->IsEmpowered())
	{
		bIsFromBack = false;
		MaxBlockCount = 20;
	}
	else
	{
		MaxBlockCount = 3;
	}

	if (OwnerCharacter->IsInPrimaryState(TAG_Character_Defense_Block) && !bIsFromBack)
	{
		if (BlockCount >= MaxBlockCount && !OwnerCharacter->HasSecondaryState(TAG_Character_BlockBreak))
		{
			OwnerCharacter->ClearAllStateTags();
			OwnerCharacter->AddSecondaryState(TAG_Character_BlockBreak);
			CachedMontageComponent->PlayBlockMontage(FName("BlockBreak"));
			BlockCount = 0;
			LastBlockTime = 0;
			CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterGuardBreak,
			                                            OwnerCharacter->GetActorLocation());

			if (!GetWorld()->GetTimerManager().IsTimerActive(DelayTimerHandle))
			{
				GetWorld()->GetTimerManager().SetTimer(
					DelayTimerHandle,
					this,
					&UMovementHandlerComponent::OnDelayedAction,
					5.0f,
					false
				);
			}

			if (ActiveShieldEffectComponent)
			{
				ActiveShieldEffectComponent->DestroyComponent();
				ActiveShieldEffectComponent = nullptr;
			}
		}
		else
		{
			HitDirection(Causer);
			const float CurrentTime = GetWorld()->GetTimeSeconds();
			LastBlockTime = CurrentTime;
			CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterGuard,
			                                            OwnerCharacter->GetActorLocation());

			CachedMontageComponent->PlayBlockMontage(FName("BlockHit"));
			++BlockCount;
		}
		return;
	}

	// 피격무적 분기
	if (OwnerCharacter->HasSecondaryState(TAG_Character_Invulnerable) && InvulnerableDuration > 0) return;

	// 피격
	OwnerCharacter->ClearStateTags({TAG_Character_Movement_Dodge}, {
		                               TAG_Character_LockOn, TAG_Character_PrepareLockOn, TAG_Character_Invulnerable,
		                               TAG_Character_Empowered});
	CachedMontageComponent->StopAllMontages(0.2f);
	CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterHit, OwnerCharacter->GetActorLocation());

	float RemoveDelay = 1.0f;

	switch (AnimType)
	{
	case EHitAnimType::HAT_FallBack:
		ToggleCameraZoom(false);
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
	case EHitAnimType::HAT_Exhausterd:
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
	case EHitAnimType::HAT_HardHit: // 중간거
		OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Medium);
		RemoveDelay = 1.0f;
		break;
	case EHitAnimType::HAT_WeakHit:
		OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Weak);
		RemoveDelay = 0.5f;
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

	// 무적 상태 진입
	OwnerCharacter->AddSecondaryState(TAG_Character_Invulnerable);
	OwnerCharacter->GetWorldTimerManager().PauseTimer(InvulnerabilityTimerHandle);
	OwnerCharacter->GetWorldTimerManager().ClearTimer(InvulnerabilityTimerHandle);
	OwnerCharacter->GetWorldTimerManager().SetTimer(InvulnerabilityTimerHandle, this,
	                                                &UMovementHandlerComponent::RemoveInvulnerability,
	                                                InvulnerableDuration, false);
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

 	if (OwnerCharacter->GetCharacterMovement()->IsFalling()) return;

	if (OwnerCharacter)
	{
		OwnerCharacter->Jump();
		CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterSpirit,
		                                            OwnerCharacter->GetActorLocation());
	}

	CachedCombatComponent->ResetCombo();
	OwnerCharacter->AddSecondaryState(TAG_Character_Movement_Jump);
}

void UMovementHandlerComponent::OnLanded(const FHitResult& Hit)
{
	OwnerCharacter->RemoveSecondaryState(TAG_Character_Movement_Jump);
	OwnerCharacter->RemoveSecondaryState(TAG_Character_Movement_OnAir);
	CachedCombatComponent->ResetCombo();
	AttackStateCount = 0;
	CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterOnGround,
	                                            OwnerCharacter->GetActorLocation());

	if (!OwnerCharacter) return;
	UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	MoveComp->BrakingDecelerationFalling = DefaultBrakingDecelerationFalling;
	MoveComp->GravityScale = DefaultGravityScale;
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

void UMovementHandlerComponent::MoveTPS(const float AxisValue, const EInputActionType ActionType) const
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_MovementBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Movement Blocked"));
		return;
	}

	if (!OwnerCharacter || FMath::IsNearlyZero(AxisValue)) return;
	
	AController* Controller = OwnerCharacter->GetController();
	if (!Controller) return;

	const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
	
	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:
		MoveForwardBackward(AxisValue, YawRotation);
		break;
	case EInputActionType::EIAT_MoveDown:
		MoveForwardBackward(-AxisValue, YawRotation);
		break;
	case EInputActionType::EIAT_MoveLeft:
		MoveLeftRight(-AxisValue, YawRotation);
		break;
	case EInputActionType::EIAT_MoveRight:
		MoveLeftRight(AxisValue, YawRotation);
		break;
	default:
		break;
	}
}

void UMovementHandlerComponent::MoveForwardBackward(const float Value, const FRotator& YawRotation) const
{
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	OwnerCharacter->AddMovementInput(Direction, Value);
}

void UMovementHandlerComponent::MoveLeftRight(const float Value, const FRotator& YawRotation) const
{
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	OwnerCharacter->AddMovementInput(Direction, Value);
}

void UMovementHandlerComponent::SetViewMode(bool bIsTPS)
{
    if (!OwnerCharacter || !OwnerCharacter->CameraBoom || !OwnerCharacter->GetCharacterMovement())
    {
        return;
    }
    
    if (bIsTPS)
    {
        OwnerCharacter->bUseControllerRotationYaw = true;
        OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
        //OwnerCharacter->CameraBoom->bUsePawnControlRotation = true;

    	OwnerCharacter->CameraBoom->SetRelativeLocation(FVector(85.f, 60.f, 60.f));
    	OwnerCharacter->CameraBoom->SetRelativeRotation(FRotator(0.f, -30.f, 0.f));
    	bActivateTPSView = true;
    }
    else
    {
    	OwnerCharacter->bUseControllerRotationYaw = false;
    	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true; // Zelda-like

    	OwnerCharacter->CameraBoom->SetRelativeLocation(FVector(-34.f, 60.f, 130.f));
    	OwnerCharacter->CameraBoom->SetRelativeRotation(FRotator(0.f, -30.f, 0.f));
    	bActivateTPSView = false;
    }
}

void UMovementHandlerComponent::ToggleCameraState(bool bLock)
{
	if (!bActivateTPSView) return;
	
	if (bLock)
	{
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		OwnerCharacter->bUseControllerRotationYaw = false;
		OwnerCharacter->CameraBoom->SetRelativeLocation_Direct(FVector(0, 0, 60));
	}
	else
	{
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		OwnerCharacter->bUseControllerRotationYaw = true;
		OwnerCharacter->CameraBoom->SetRelativeLocation_Direct(FVector(85, 60, 70));
	}
}

bool UMovementHandlerComponent::ParryCheck()
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

		return true;
	}

	return false;
}

void UMovementHandlerComponent::Attack()
{
	UAnimMontage* Montage = nullptr;
	FName SectionName;

	if (ActiveShieldEffectComponent)
	{
		ActiveShieldEffectComponent->DestroyComponent();
		ActiveShieldEffectComponent = nullptr;
	}

	if (CameraFocusTarget
		&& !CameraFocusTarget->IsA(ASLAIBaseCharacter::StaticClass())
		&& !OwnerCharacter->IsInPrimaryState(TAG_Character_HitReaction))
	{
		const bool bIsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();
		const float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), CameraFocusTarget->GetActorLocation());

		if (!bIsFalling && OwnerCharacter->HasSecondaryState(TAG_Character_LockOn) && Distance <
			ExecuteDistanceThreshold)
		{
			const FVector TargetForward = CameraFocusTarget->GetActorForwardVector();
			const FVector ToOwner = (OwnerCharacter->GetActorLocation() - CameraFocusTarget->GetActorLocation()).
				GetSafeNormal();

			const float Dot = FVector::DotProduct(TargetForward, ToOwner);
			if (Dot < -0.94f)
			{
				//Execution();
				return;
			}
		}
	}

	if (CameraFocusTarget && !bDidBeginAttack)
	{
		BeginAttack();
		bDidBeginAttack = true;
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

void UMovementHandlerComponent::BeginAttack()
{
	const bool bIsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();
	const float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), CameraFocusTarget->GetActorLocation());

	// Lock On 상태일때 분기
	if (OwnerCharacter->HasSecondaryState(TAG_Character_LockOn) && Distance > WarpDistanceThreshold)
	{
		const FVector TargetForward = CameraFocusTarget->GetActorForwardVector();
		const FVector TargetLoc = CameraFocusTarget->GetActorLocation() + TargetForward * 120.f;

		const FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(
			TargetLoc, CameraFocusTarget->GetActorLocation());

		if (UMotionWarpingComponent* WarpComp = Cast<UMotionWarpingComponent>(
			OwnerCharacter->GetComponentByClass(UMotionWarpingComponent::StaticClass())))
		{
			WarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Warp"), TargetLoc, TargetRot);
		}

		if (bIsFalling)
		{
			ToggleCameraZoom(false);
			CachedMontageComponent->PlayTrickMontage("BeginAir");
		}
		else
		{
			ToggleCameraZoom(false);
			CachedMontageComponent->PlayTrickMontage("Begin");
		}

		OwnerCharacter->SetPrimaryState(TAG_Character_Attack);
	}
}

void UMovementHandlerComponent::Execution()
{
	struct FExecutionData
	{
		EAttackAnimType AttackType;
		FName MontageName;
		FGameplayTag SecondaryTag;
	};

	TArray<FExecutionData> ExecutionOptions = {
		{EAttackAnimType::AAT_FinalAttackA, FName("ExecutionA"), TAG_Character_Attack_ExecutionA},
		{EAttackAnimType::AAT_FinalAttackB, FName("ExecutionB"), TAG_Character_Attack_ExecutionB},
		{EAttackAnimType::AAT_FinalAttackC, FName("ExecutionC"), TAG_Character_Attack_ExecutionC}
	};

	const int32 RandomIndex = FMath::RandRange(0, ExecutionOptions.Num() - 1);
	const FExecutionData& ChosenExecution = ExecutionOptions[RandomIndex];

	const FVector TargetForward = CameraFocusTarget->GetActorForwardVector();
	const FVector TargetLoc = CameraFocusTarget->GetActorLocation() + -TargetForward * 80.f;
	const FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(TargetLoc, CameraFocusTarget->GetActorLocation());

	if (UMotionWarpingComponent* WarpComp = Cast<UMotionWarpingComponent>(
		OwnerCharacter->GetComponentByClass(UMotionWarpingComponent::StaticClass())))
	{
		WarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Warp"), TargetLoc, TargetRot);
	}

	USlowMotionHelper::ApplyZoomWithSlowMotion(this, 0.2f, 0.3f, 80.0f);
	CachedMontageComponent->PlayExecutionMontage(ChosenExecution.MontageName);
	OwnerCharacter->AddSecondaryState(ChosenExecution.SecondaryTag);
}

void UMovementHandlerComponent::Blast(const EItemType ItemType)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AirBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Blast Blocked"));
		return;
	}

	switch (ItemType)
	{
	case EItemType::IT_Sword:
		CachedMontageComponent->PlaySkillMontage(FName("BlastSword"));
		break;
	case EItemType::IT_Shield:
		CachedMontageComponent->PlaySkillMontage(FName("BlastShield"));
		break;
	}

	OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Blast);
	OwnerCharacter->SetPrimaryState(TAG_Character_Attack);
}

void UMovementHandlerComponent::ApplyAttackState(const FName& SectionName, bool bIsFalling)
{
	if (bIsFalling && AttackStateCount != 3)
	{
		FixCharacterVelocity();
		OwnerCharacter->AddSecondaryState(TAG_Character_Movement_OnAir);
		++AttackStateCount;
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
			//CachedCombatComponent->StartCharging();
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

void UMovementHandlerComponent::OnDelayedAction()
{
	OwnerCharacter->RemoveSecondaryState(TAG_Character_BlockBreak);
	GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
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

void UMovementHandlerComponent::DodgeLoco()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_DogeBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Dodge Blocked"));
		return;
	}

	ToggleCameraZoom(false);

	FVector DesiredDodgeDirection;
	const FVector InputDirection = OwnerCharacter->GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal();
	if (!InputDirection.IsNearlyZero())
	{
		DesiredDodgeDirection = InputDirection;
	}
	else
	{
		DesiredDodgeDirection = -OwnerCharacter->GetActorForwardVector();
	}

	const FVector CharacterForward = OwnerCharacter->GetActorForwardVector();
	const FVector CharacterRight = OwnerCharacter->GetActorRightVector();

	const double ForwardDotTemp = FVector::DotProduct(CharacterForward, DesiredDodgeDirection);
	const double RightDotTemp = FVector::DotProduct(CharacterRight, DesiredDodgeDirection);

	constexpr double AngleThreshold45 = 0.707;

	EDodgeDirection DirectionEnum;

	if (ForwardDotTemp > AngleThreshold45)
	{
		DirectionEnum = EDodgeDirection::Forward;
	}
	else if (ForwardDotTemp < -AngleThreshold45)
	{
		DirectionEnum = EDodgeDirection::Backward;
	}
	else
	{
		if (RightDotTemp > 0)
		{
			DirectionEnum = EDodgeDirection::Right;
		}
		else
		{
			DirectionEnum = EDodgeDirection::Left;
		}
	}

	const double AngleRad = FMath::Acos(ForwardDotTemp);
	double AngleDeg = FMath::RadiansToDegrees(AngleRad);

	if (RightDotTemp < 0)
	{
		AngleDeg *= -1.0;
	}

	if (AngleDeg >= -22.5 && AngleDeg < 22.5) DirectionEnum = EDodgeDirection::Forward;
	else if (AngleDeg >= 22.5 && AngleDeg < 67.5) DirectionEnum = EDodgeDirection::ForwardRight;
	else if (AngleDeg >= 67.5 && AngleDeg < 112.5) DirectionEnum = EDodgeDirection::Right;
	else if (AngleDeg >= 112.5 && AngleDeg < 157.5) DirectionEnum = EDodgeDirection::BackwardRight;
	else if (AngleDeg >= 157.5 || AngleDeg < -157.5) DirectionEnum = EDodgeDirection::Backward;
	else if (AngleDeg >= -157.5 && AngleDeg < -112.5) DirectionEnum = EDodgeDirection::BackwardLeft;
	else if (AngleDeg >= -112.5 && AngleDeg < -67.5) DirectionEnum = EDodgeDirection::Left;
	else if (AngleDeg >= -67.5 && AngleDeg < -22.5) DirectionEnum = EDodgeDirection::ForwardLeft;

	FName MontageToPlay = "";
	switch (DirectionEnum)
	{
	case EDodgeDirection::Forward: MontageToPlay = "Forward";
		break;
	case EDodgeDirection::ForwardRight: MontageToPlay = "ForwardRight";
		break;
	case EDodgeDirection::Right: MontageToPlay = "Right";
		break;
	case EDodgeDirection::BackwardRight: MontageToPlay = "BackwardRight";
		break;
	case EDodgeDirection::Backward: MontageToPlay = "Backward";
		break;
	case EDodgeDirection::BackwardLeft: MontageToPlay = "BackwardLeft";
		break;
	case EDodgeDirection::Left: MontageToPlay = "Left";
		break;
	case EDodgeDirection::ForwardLeft: MontageToPlay = "ForwardLeft";
		break;
	}

	CachedMontageComponent->PlayDodgeMontage(MontageToPlay);
	OwnerCharacter->ClearStateTags({}, {TAG_Character_PrepareLockOn, TAG_Character_LockOn, TAG_Character_Empowered});
	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Dodge);
	CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterDodge,
	                                            OwnerCharacter->GetActorLocation());
}

void UMovementHandlerComponent::ToggleLockState()
{
	if (OwnerCharacter->HasSecondaryState(TAG_Character_PrepareLockOn))
	{
		OwnerCharacter->RemoveSecondaryState(TAG_Character_PrepareLockOn);
		OwnerCharacter->DisableLockOnMode();
		if (const APawn* Pawn = Cast<APawn>(CameraFocusTarget))
		{
			if (AMonsterAIController* AIController = Cast<AMonsterAIController>(Pawn->GetController()))
			{
				AIController->ToggleLockOnWidget(false);
			}
		}
		CameraFocusTarget = nullptr;
		bDidBeginAttack = false;
	}
	else
	{
		OwnerCharacter->AddSecondaryState(TAG_Character_PrepareLockOn);
	}
}

void UMovementHandlerComponent::DisableLock()
{
	if (const APawn* Pawn = Cast<APawn>(CameraFocusTarget))
	{
		if (AMonsterAIController* AIController = Cast<AMonsterAIController>(Pawn->GetController()))
		{
			AIController->ToggleLockOnWidget(false);
		}
	}

	CameraFocusTarget = nullptr;
	OwnerCharacter->DisableLockOnMode();
	bDidBeginAttack = false;
}

void UMovementHandlerComponent::BeginBuff()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_BuffBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USL25DMovementHandlerComponent: Buff Blocked"));
		return;
	}
	
	ToggleCameraZoom(false);
	const bool bIsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();

	OwnerCharacter->ClearStateTags({TAG_Character_OnBuff}, {TAG_Character_PrepareLockOn, TAG_Character_LockOn, TAG_Character_Empowered});
	OwnerCharacter->SetPrimaryState(TAG_Character_OnBuff);
	CachedMontageComponent->StopAllMontages(0.2f);

	if (bIsFalling)
	{
		CachedMontageComponent->PlayTrickMontage("BuffAir");
		FixCharacterVelocity();
	}
	else
	{
		CachedMontageComponent->PlayTrickMontage("Buff");
	}
}

void UMovementHandlerComponent::RotateCameraToTarget(const AActor* Target, float DeltaTime)
{
	if (APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		const FVector CameraLocation = OwnerCharacter->CameraBoom->GetComponentLocation();
		const FVector TargetLocation = Target->GetActorLocation();

		float DistanceToTarget = FVector::Dist(CameraLocation, TargetLocation);
		if (DistanceToTarget < 100.0f) return;

		FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);
		DesiredRotation.Pitch += 10.0f;
		DesiredRotation.Pitch = FMath::Clamp(DesiredRotation.Pitch, -30.0f, 30.0f);
		DesiredRotation.Roll = 0.0f;

		const FRotator CurrentRotation = PC->GetControlRotation();
		const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, 4.0f);

		PC->SetControlRotation(NewRotation);
	}
}

void UMovementHandlerComponent::ToggleCameraZoom(const bool bIsZoomedOut, const float ZoomedOutArmLength)
{
	if (!OwnerCharacter->CameraBoom || !OwnerCharacter->CameraBoom || OwnerCharacter->HasSecondaryState(
		TAG_Character_LockOn))
		return;

	if (bIsZoomedOut)
	{
		DesiredArmLength = DefaultArmLength;
	}
	else
	{
		DesiredArmLength = ZoomedOutArmLength;
	}
}

void UMovementHandlerComponent::SpawnSword()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AirBlock) || OwnerCharacter->GetMovementComponent()->
		IsFalling())
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Dodge Blocked"));
		return;
	}

	if (!CachedCombatComponent->IsEmpowered()) return;

	ToggleCameraZoom(false, 1000.f);
	CachedMontageComponent->PlaySkillMontage("SwordFromSky");
	OwnerCharacter->SetPrimaryState(TAG_Character_Attack_SpawnSword);
	CachedCombatComponent->ResetCombatMode();

	USlowMotionHelper::ApplyActorSlowMotion(OwnerCharacter, 0.2f, 1.0f);
}

void UMovementHandlerComponent::Airborne()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AirBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Dodge Blocked"));
		return;
	}

	ToggleCameraZoom(false);
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

	ToggleCameraZoom(false);
	CachedMontageComponent->PlaySkillMontage(FName("AirUp"));

	OwnerCharacter->AddSecondaryState(TAG_Character_Attack_Airup);
	OwnerCharacter->SetPrimaryState(TAG_Character_Attack);
}

void UMovementHandlerComponent::AirDown()
{
	ToggleCameraZoom(false);
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
		CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterBeginDefence,
		                                            OwnerCharacter->GetActorLocation());

		if (CachedCombatComponent->IsEmpowered())
		{
			if (EmpoweredShieldEffect)
			{
				if (ActiveShieldEffectComponent)
				{
					ActiveShieldEffectComponent->DestroyComponent();
				}

				ActiveShieldEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
					EmpoweredShieldEffect,
					OwnerCharacter->GetRootComponent(),
					NAME_None,
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::KeepRelativeOffset,
					true
				);
			}
		}
	}
	else
	{
		OwnerCharacter->RemovePrimaryState(TAG_Character_Defense_Block);
		CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterEndDefence,
		                                            OwnerCharacter->GetActorLocation());

		if (ActiveShieldEffectComponent)
		{
			ActiveShieldEffectComponent->DestroyComponent();
			ActiveShieldEffectComponent = nullptr;
		}
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
		BeginBuff();
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
	case ECharacterMontageState::ECS_Buff:
	case ECharacterMontageState::ECS_BuffAir:
		CachedCombatComponent->SetEmpoweredCombatMode(20);
		OwnerCharacter->GetCharacterMovement()->GravityScale = 1.f;
		break;
	case ECharacterMontageState::ECS_Attack_BlastSword:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Blast);
		break;
	case ECharacterMontageState::ECS_Attack_BlastShield:
		OwnerCharacter->RemoveSecondaryState(TAG_Character_Attack_Blast);
		break;
	case ECharacterMontageState::ECS_ETC:
		OwnerCharacter->ChangeVisibilityWeapons(true);
		break;
	case ECharacterMontageState::ECS_Attack_SpawnSword:
		CachedMontageComponent->PlaySkillMontage("Dodge");
		break;
	case ECharacterMontageState::ECS_Attack_SpawnSwordDodge:
		break;
	default:
		break;
	}

	ToggleCameraZoom(true);
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
		SpawnSword();
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
	case ESkillType::ST_BlastSword:
		Blast(EItemType::IT_Sword);
		break;
	case ESkillType::ST_BlastShield:
		Blast(EItemType::IT_Shield);
		break;
	default:
		break;
	}
}
