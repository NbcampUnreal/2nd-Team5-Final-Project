#include "SLMovementHandlerComponent.h"

#include "Character/SLBaseCharacter.h"
#include "Character/SLCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
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

	OwnerCharacter = Cast<ASLCharacter>(GetOwner());
	
	if (OwnerCharacter)
	{
		CachedMontageComponent = OwnerCharacter->FindComponentByClass<UAnimationMontageComponent>();
		CachedBattleComponent = OwnerCharacter->FindComponentByClass<UBattleComponent>();
		BindIMCComponent();
	}
}

void UMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
			BufferComp->AddBufferedInput(ActionType);
		}
		//Attack();
		break;
	case EInputActionType::EIAT_PointMove:
		PointMove();
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
	case EInputActionType::EIAT_Interaction:
	case EInputActionType::EIAT_Attack:
	case EInputActionType::EIAT_PointMove:
	case EInputActionType::EIAT_Menu:
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
	if (!OwnerCharacter || Value.IsNearlyZero()) return;

	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC) return;

	FRotator ControlRot = PC->GetControlRotation();

	ControlRot.Yaw += Value.X * MouseSensitivity;
	ControlRot.Pitch -= Value.Y * MouseSensitivity;
	ControlRot.Pitch = FMath::Clamp(ControlRot.Pitch, MinPitch, MaxPitch);

	ControlRot.Roll = 0.f;

	PC->SetControlRotation(ControlRot);
}

void UMovementHandlerComponent::Jump()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump"));
	if (OwnerCharacter) OwnerCharacter->Jump();
}

void UMovementHandlerComponent::Move(const float AxisValue, const EInputActionType ActionType)
{
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
	
	// Test
	if (auto* CMC = GetOwner()->FindComponentByClass<UCameraManagerComponent>())
	{
		static const TArray<EGameCameraType> CameraOrder = {
			EGameCameraType::EGCT_Default,
			EGameCameraType::EGCT_Battle,
			EGameCameraType::EGCT_TopDown,
			EGameCameraType::EGCT_SideView
		};

		CurrentIndex = (CurrentIndex + 1) % CameraOrder.Num();
		const EGameCameraType NextType = CameraOrder[CurrentIndex];

		CMC->SwitchCamera(NextType);
	}
}

void UMovementHandlerComponent::Attack()
{
	if (CachedMontageComponent)
	{
		CachedMontageComponent->PlayAttackMontage(FName("Attack1"));
	}
	
	if (CachedBattleComponent)
	{
		CachedBattleComponent->PerformAttack();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleComponent not found on %s"), *GetOwner()->GetName());
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

EGameCameraType UMovementHandlerComponent::GetCurrentCameraType() const
{
	if (auto* CMC = GetOwner()->FindComponentByClass<UCameraManagerComponent>())
	{
		return CMC->GetCurrentCameraType();
	}

	return EGameCameraType::EGCT_Default;
}
