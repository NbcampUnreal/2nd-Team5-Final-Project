#include "SLMovementHandlerComponent.h"

#include "Character/SLBaseCharacter.h"
#include "Character/SLCharacter.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/PlayerState/SLBattlePlayerState.h"

UMovementHandlerComponent::UMovementHandlerComponent(): OwnerCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ASLCharacter>(GetOwner());

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is not a character! Component will not function."));
		return;
	}

	BindIMCComponent();
}

void UMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMovementHandlerComponent::OnActionTriggered(EInputActionType ActionType, FInputActionValue Value)
{
	FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	EnumName.RemoveFromStart("EInputActionType::");

	UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionTriggered → %s"), *EnumName);
	
	switch (ActionType)
	{
	case EInputActionType::EIAT_Look:
		Look(Value.Get<FVector2D>());
		break;
	case EInputActionType::EIAT_MoveUp:
		Move(Value.Get<float>(), FVector::ForwardVector, ActionType);
		break;
	case EInputActionType::EIAT_MoveDown:
		Move(Value.Get<float>(), -FVector::ForwardVector, ActionType);
		break;
	case EInputActionType::EIAT_MoveLeft:
		Move(Value.Get<float>(), -FVector::RightVector, ActionType);
		break;
	case EInputActionType::EIAT_MoveRight:
		Move(Value.Get<float>(), FVector::RightVector, ActionType);
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::OnActionStarted(EInputActionType ActionType)
{
	FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	EnumName.RemoveFromStart("EInputActionType::");

	UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionStarted → %s"), *EnumName);

	switch (ActionType)
	{
	case EInputActionType::EIAT_Jump:
		Jump();
		break;

	case EInputActionType::EIAT_Interaction:
		Interact();
		break;

	case EInputActionType::EIAT_Attack:
		Attack();
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
	FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	EnumName.RemoveFromStart("EInputActionType::");

	UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionCompleted → %s"), *EnumName);

	switch (ActionType)
	{
	case EInputActionType::EIAT_Jump:
		break;

	case EInputActionType::EIAT_Interaction:
		break;

	case EInputActionType::EIAT_Attack:
		break;

	case EInputActionType::EIAT_PointMove:
		break;

	case EInputActionType::EIAT_Walk:
		ToggleWalk(false);
		break;

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

	AController* Controller = OwnerCharacter->GetController();
	if (!Controller) return;

	OwnerCharacter->AddControllerYawInput(Value.X);
	OwnerCharacter->AddControllerPitchInput(Value.Y);
}

void UMovementHandlerComponent::Jump()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump"));
	if (OwnerCharacter) OwnerCharacter->Jump();
}

void UMovementHandlerComponent::Move(const float AxisValue, const FVector& Direction, EInputActionType ActionType)
{
	if (!OwnerCharacter || FMath::IsNearlyZero(AxisValue)) return;

	const FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

	const FVector WorldDirection = YawRotation.RotateVector(Direction);

	if (ActionType == EInputActionType::EIAT_MoveUp)
	{
		const FRotator TargetRot = FRotationMatrix::MakeFromX(WorldDirection).Rotator();
		OwnerCharacter->SetActorRotation(TargetRot);
	}

	else if (ActionType == EInputActionType::EIAT_MoveDown)
	{
		const FRotator TargetRot = FRotationMatrix::MakeFromX(-WorldDirection).Rotator();
		OwnerCharacter->SetActorRotation(TargetRot);
	}

	OwnerCharacter->AddMovementInput(WorldDirection, AxisValue);
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
	UE_LOG(LogTemp, Log, TEXT("Attack triggered"));
	// TODO: 무기/애니메이션 처리 연결
	
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
