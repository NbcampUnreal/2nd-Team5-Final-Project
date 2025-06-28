#include "SL25DMovementHandlerComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "SLMovementHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Character/SlowMotionHelper/SlowMotionHelper.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "SubSystem/SLSoundTypes.h"


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
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		OwnerCharacter->CameraBoom->bInheritPitch = false;
		OwnerCharacter->CameraBoom->bInheritYaw = false;
		OwnerCharacter->CameraBoom->bInheritRoll = false;
		OwnerCharacter->bUseControllerRotationYaw = false;
		OwnerCharacter->CameraBoom->bEnableCameraLag = false;
		OwnerCharacter->CameraBoom->bEnableCameraRotationLag = false;
		OwnerCharacter->CameraBoom->bDoCollisionTest = false;

		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 700.0f;
		OwnerCharacter->GetCapsuleComponent()->OnComponentHit.AddDynamic(
			this, &USL25DMovementHandlerComponent::OnCharacterHit);
	}
}

void USL25DMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleRotation(DeltaTime);

	if (bIsMovingToTarget)
	{
		MoveToTarget(DeltaTime);
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

void USL25DMovementHandlerComponent::OnActionTriggered_Implementation(EInputActionType ActionType,
                                                                      FInputActionValue Value)
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

void USL25DMovementHandlerComponent::OnActionStarted_Implementation(EInputActionType ActionType)
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
		break;
	case EInputActionType::EIAT_Interaction:
		BeginBuff();
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

				//BeginBuff();
				CachedCombatComponent->SetEmpoweredCombatMode(20);
				USlowMotionHelper::ApplyGlobalSlowMotion(OwnerCharacter, 0.2f, 0.3f);

				return;
			}
			Attack();
			break;
		}
	case EInputActionType::EIAT_PointMove:
		if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_PointMoveBlock))
		{
			//UE_LOG(LogTemp, Warning, TEXT("USL25DMovementHandlerComponent: Point Move Blocked"));
			return;
		}
		//StartFacingMouse();
		StartMoveToMouseCursorLocation();
		break;
	case EInputActionType::EIAT_Block:
		if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
		{
			//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Input Blocked"));
			return;
		}
		Block(true);
		break;
	case EInputActionType::EIAT_Walk:
		DodgeLoco();
		break;
	case EInputActionType::EIAT_Menu:
	case EInputActionType::EIAT_LockObject:
		break;

	default:
		break;
	}
}

void USL25DMovementHandlerComponent::OnActionCompleted_Implementation(EInputActionType ActionType)
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
		Block(false);
		break;

	default:
		break;
	}
}

void USL25DMovementHandlerComponent::OnHitReceived_Implementation(AActor* Causer, float Damage,
                                                                  const FHitResult& HitResult,
                                                                  const EHitAnimType AnimType)
{
	Super::OnHitReceived_Implementation(Causer, Damage, HitResult, AnimType);

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_HitBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USLMovementComponentBase: Hit Blocked"));
		return;
	}

	if (OwnerCharacter->HasSecondaryState(TAG_Character_Defense_Parry)
		|| OwnerCharacter->IsInPrimaryState(TAG_Character_OnBuff)
		|| OwnerCharacter->HasSecondaryState(TAG_Character_Attack_Blast))
		return;

	bool bIsFromBack = false;
	FRotator TargetRotation;
	RotateToHitCauser(Causer, TargetRotation, bIsFromBack);

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
			CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterGuard,
			                                            OwnerCharacter->GetActorLocation());
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
	OwnerCharacter->ClearStateTags({TAG_Character_OnBuff}, {
		                               TAG_Character_LockOn, TAG_Character_PrepareLockOn, TAG_Character_Invulnerable,
		                               TAG_Character_Empowered
	                               });
	CachedMontageComponent->StopAllMontages(0.2f);
	CachedBattleSoundSubsystem->PlayBattleSound(EBattleSoundType::BST_CharacterHit, OwnerCharacter->GetActorLocation());

	switch (AnimType)
	{
	case EHitAnimType::HAT_FallBack:
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
	case EHitAnimType::HAT_Exhausterd: // 기절
		if (OwnerCharacter->GetCharacterMovement()->IsFalling())
		{
			OwnerCharacter->SetActorRotation(TargetRotation);
			CachedMontageComponent->PlayHitMontage(FName("Fall"));
			OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Falling);
		}
		else
		{
			OwnerCharacter->SetActorRotation(TargetRotation);
			CachedMontageComponent->PlayHitMontage(FName("Groggy"));
			OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Groggy);
		}
		return;
	case EHitAnimType::HAT_HardHit:
	case EHitAnimType::HAT_WeakHit:
		break;
	default: break;
	}

	HitDirection(Causer);
}

void USL25DMovementHandlerComponent::OnDelayedAction()
{
	OwnerCharacter->RemoveSecondaryState(TAG_Character_BlockBreak);
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

void USL25DMovementHandlerComponent::RotateToHitCauser(const AActor* Causer, FRotator& TargetRotation,
                                                       bool& bIsHitFromBack)
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

void USL25DMovementHandlerComponent::BeginBuff()
{
	if (!CachedMontageComponent || !OwnerCharacter || !CachedCombatComponent) return;

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_BuffBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USL25DMovementHandlerComponent: Buff Blocked"));
		return;
	}

	OwnerCharacter->ClearStateTags({}, {TAG_Character_PrepareLockOn, TAG_Character_LockOn, TAG_Character_Empowered});
	OwnerCharacter->SetPrimaryState(TAG_Character_OnBuff);
	CachedMontageComponent->StopAllMontages(0.2f);
	CachedMontageComponent->PlayTrickMontage("Buff");
}

void USL25DMovementHandlerComponent::DodgeLoco()
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_DogeBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Dodge Blocked"));
		return;
	}

	FaceMouseCursorInstantly();
	CachedMontageComponent->PlayDodgeMontage("Forward");
	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Dodge);
}

void USL25DMovementHandlerComponent::Block(const bool bIsBlocking)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_DefenceBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Defence Blocked"));
		return;
	}

	FaceMouseCursorInstantly();

	if (bIsBlocking && !OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		OwnerCharacter->SetPrimaryState(TAG_Character_Defense_Block);
	}
	else
	{
		OwnerCharacter->RemovePrimaryState(TAG_Character_Defense_Block);
	}
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

	FaceMouseCursorInstantly();

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

void USL25DMovementHandlerComponent::StartMoveToMouseCursorLocation()
{
	APlayerController* PlayerController = OwnerCharacter
		                                      ? Cast<APlayerController>(OwnerCharacter->GetController())
		                                      : nullptr;
	if (!PlayerController) return;

	FVector WorldLocation, WorldDirection;
	if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		return;
	}

	FaceMouseCursorInstantly();
	const FPlane CharacterPlane(OwnerCharacter->GetActorLocation(), FVector::UpVector);
	const FVector IntersectionPoint = FMath::LinePlaneIntersection(
		WorldLocation,
		WorldLocation + (WorldDirection * 15000.0f),
		CharacterPlane
	);

	TargetMoveLocation = IntersectionPoint;
	bIsMovingToTarget = true;

	if (MoveTargetEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			MoveTargetEffect,
			TargetMoveLocation,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true,
			true
		);
	}

	GetWorld()->GetTimerManager().SetTimer(
		FacingMouseTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			StopFacingMouse();
		}),
		0.2, false
	);
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

	// 콤보 초기화 로직
	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		ComboResetTimerHandle,
		this,
		&USL25DMovementHandlerComponent::ResetCombo,
		ComboResetTime,
		false
	);
}

void USL25DMovementHandlerComponent::ResetCombo()
{
	if (CachedCombatComponent)
	{
		CachedCombatComponent->ResetCombo();
	}

	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
}

void USL25DMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_MovementBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Movement Blocked"));
		return;
	}

	if (!OwnerCharacter || FMath::IsNearlyZero(AxisValue)) return;

	const FVector ForwardDir = FVector(1.f, -1.f, 0.f).GetSafeNormal();
	const FVector RightDir = FVector(1.f, 1.f, 0.f).GetSafeNormal();

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

	bIsMovingToTarget = false;
}

void USL25DMovementHandlerComponent::FaceMouseCursorInstantly() const
{
	if (!OwnerCharacter) return;

	APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PlayerController) return;

	FVector WorldLocation, WorldDirection;
	if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		return;
	}

	const FPlane CharacterPlane(OwnerCharacter->GetActorLocation(), FVector::UpVector);

	const FVector IntersectionPoint = FMath::LinePlaneIntersection(
		WorldLocation,
		WorldLocation + (WorldDirection * 15000.0f),
		CharacterPlane
	);

	const FVector DirectionToTarget = IntersectionPoint - OwnerCharacter->GetActorLocation();
	const FVector FlattenedDirection = FVector(DirectionToTarget.X, DirectionToTarget.Y, 0.0f);

	if (!FlattenedDirection.IsNearlyZero())
	{
		const FRotator TargetRotation = FlattenedDirection.Rotation();

		OwnerCharacter->SetActorRotation(FRotator(0.0f, TargetRotation.Yaw, 0.0f));
	}
}

void USL25DMovementHandlerComponent::OnCharacterHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                                    const FHitResult& Hit)
{
	if (!bIsMovingToTarget) return;

	if (!OtherActor || OtherActor == OwnerCharacter)
	{
		return;
	}

	if (OtherActor->IsA(APawn::StaticClass()))
	{
		return;
	}

	if (!OwnerCharacter->GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}

	if (OwnerCharacter->GetVelocity().Length() < 100)
	{
		OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
		return;
	}

	const FVector HitNormal = Hit.ImpactNormal;

	const float DotProduct = FVector::DotProduct(OwnerCharacter->GetActorForwardVector(), HitNormal);

	if (DotProduct < -0.7f)
	{
		OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
		if (bIsMovingToTarget)
		{
			bIsMovingToTarget = false;
		}

		FRotator TargetRotation = (-HitNormal).Rotation();
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;
		OwnerCharacter->SetActorRotation(TargetRotation);

		if (CachedMontageComponent)

			OwnerCharacter->SetPrimaryState(TAG_Character_HitReaction_Knockback);
		CachedMontageComponent->PlayHitMontage(FName("KnockBack"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Character hit a wall head-on!"));
}

void USL25DMovementHandlerComponent::HandleRotation(float DeltaTime)
{
	if (!bShouldFaceMouse)
	{
		return;
	}

	if (!OwnerCharacter) return;

	APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PlayerController) return;

	FVector WorldLocation, WorldDirection;
	if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		return;
	}

	const FPlane CharacterPlane(OwnerCharacter->GetActorLocation(), FVector::UpVector);
	const FVector IntersectionPoint = FMath::LinePlaneIntersection(
		WorldLocation,
		WorldLocation + (WorldDirection * 15000.0f),
		CharacterPlane
	);

	const FVector DirectionToTarget = IntersectionPoint - OwnerCharacter->GetActorLocation();
	const FVector FlattenedDirection = FVector(DirectionToTarget.X, DirectionToTarget.Y, 0.0f);

	if (!FlattenedDirection.IsNearlyZero())
	{
		const FRotator TargetRotation = FlattenedDirection.Rotation();
		const FRotator CorrectedTargetRotation = FRotator(0.0f, TargetRotation.Yaw - 90.0f, 0.0f);

		const FRotator CurrentRotation = OwnerCharacter->GetActorRotation();

		const FRotator SmoothedRotation = FMath::RInterpTo(
			CurrentRotation,
			CorrectedTargetRotation,
			DeltaTime,
			RotationSpeed
		);

		OwnerCharacter->SetActorRotation(FRotator(0.0f, SmoothedRotation.Yaw, 0.0f));
	}

	//DrawDebugLine(GetWorld(), WorldLocation, IntersectionPoint, FColor::Green, false, -1.0f, 0, 1.0f);
	//DrawDebugSphere(GetWorld(), IntersectionPoint, 25.0f, 12, FColor::Red, false, -1.0f, 0, 1.0f);
	//DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation(), IntersectionPoint, FColor::Yellow, false, -1.0f, 0, 2.0f);
}

void USL25DMovementHandlerComponent::MoveToTarget(float DeltaTime)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_MovementBlock))
	{
		bIsMovingToTarget = false;
		return;
	}

	const FVector CurrentLocation = OwnerCharacter->GetActorLocation();

	if (FVector::DistSquared2D(CurrentLocation, TargetMoveLocation) < FMath::Square(AcceptanceRadius))
	{
		bIsMovingToTarget = false;
		return;
	}

	FVector MoveDirection = (TargetMoveLocation - CurrentLocation);
	MoveDirection.Z = 0.0f;
	MoveDirection.Normalize();

	if (!MoveDirection.IsNearlyZero())
	{
		OwnerCharacter->AddMovementInput(MoveDirection, 1.0f);
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
		//OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
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
	case ECharacterMontageState::ECS_Buff:
		CachedCombatComponent->SetEmpoweredCombatMode(10);
		break;
	case ECharacterMontageState::ECS_ETC:
		OwnerCharacter->ChangeVisibilityWeapons(true);
		break;
	default:
		break;
	}

	StopFacingMouse();
	OwnerCharacter->SetPrimaryState(TAG_Character_Movement_Idle);
}
