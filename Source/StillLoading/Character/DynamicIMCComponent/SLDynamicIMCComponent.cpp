#include "SLDynamicIMCComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/SLPlayerCharacterBase.h"

UDynamicIMCComponent::UDynamicIMCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentIMC = nullptr;
}

void UDynamicIMCComponent::BeginPlay()
{
	Super::BeginPlay();
	BindDefaultSetting();
}

void UDynamicIMCComponent::BindDefaultSetting()
{
	for (const FInputBindingData& Binding : ActionBindings)
	{
		if (Binding.Action)
		{
			ActionMap.Add(Binding.ActionType, Binding.Action);
		}
	}

	const ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	const APlayerController* PC = OwnerChar ? Cast<APlayerController>(OwnerChar->GetController()) : nullptr;
	if (!PC || !DefaultIMC) return;

	auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem) return;

	Subsystem->AddMappingContext(DefaultIMC, 0);
	CurrentIMC = DefaultIMC;

	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		for (const auto& Pair : ActionMap)
		{
			const EInputActionType ActionType = Pair.Key;
			const UInputAction* Action = Pair.Value;
			if (!Action) continue;

			BindInputForAction(Action, ActionType, InputComp);
		}
	}
}

void UDynamicIMCComponent::SetKeyForAction(EInputActionType ActionType, const FKey& NewKey)
{
	FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	EnumName.RemoveFromStart("EInputActionType::");
	UE_LOG(LogTemp, Display, TEXT("SetKeyForAction: %s → %s"), *EnumName, *NewKey.GetDisplayName().ToString());

	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem) return;

	if (CurrentIMC)
	{
		Subsystem->RemoveMappingContext(CurrentIMC);
	}

	CurrentIMC = NewObject<UInputMappingContext>(
		this,
		MakeUniqueObjectName(this, UInputMappingContext::StaticClass(), TEXT("DynamicIMC"))
	);

	if (UInputAction* Action = ActionMap.FindRef(ActionType))
	{
		CurrentIMC->MapKey(Action, NewKey);

		if (ASLPlayerCharacterBase* OwnerChar = Cast<ASLPlayerCharacterBase>(GetOwner()))
		{
			if (auto* InputComp = Cast<UEnhancedInputComponent>(OwnerChar->InputComponent))
			{
				BindInputForAction(Action, ActionType, InputComp);
			}
		}
	}

	Subsystem->AddMappingContext(CurrentIMC, 0);
}

void UDynamicIMCComponent::BindInputForAction(const UInputAction* Action, const EInputActionType ActionType, UEnhancedInputComponent* InputComp)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_MoveUp:
	case EInputActionType::EIAT_MoveDown:
	case EInputActionType::EIAT_MoveLeft:
	case EInputActionType::EIAT_MoveRight:
		InputComp->BindAction(Action, ETriggerEvent::Completed, this, &UDynamicIMCComponent::HandleActionCompleted);
	case EInputActionType::EIAT_Look:
		InputComp->BindAction(Action, ETriggerEvent::Triggered, this, &UDynamicIMCComponent::HandleActionTriggered);
		break;

	case EInputActionType::EIAT_Walk:
	case EInputActionType::EIAT_Jump:
	case EInputActionType::EIAT_Interaction:
	case EInputActionType::EIAT_Attack:
	case EInputActionType::EIAT_PointMove:
	case EInputActionType::EIAT_Menu:
	case EInputActionType::EIAT_Block:
		InputComp->BindAction(Action, ETriggerEvent::Started, this, &UDynamicIMCComponent::HandleActionStarted);
		InputComp->BindAction(Action, ETriggerEvent::Completed, this, &UDynamicIMCComponent::HandleActionCompleted);
		break;

	default:
		break;
	}
}

void UDynamicIMCComponent::HandleActionTriggered(const FInputActionInstance& Instance)
{
	if (const UInputAction* TriggeredAction = Instance.GetSourceAction())
	{
		for (const auto& Pair : ActionMap)
		{
			if (Pair.Value == TriggeredAction)
			{
				OnActionTriggered.Broadcast(Pair.Key, Instance.GetValue());
				break;
			}
		}
	}
}

void UDynamicIMCComponent::HandleActionStarted(const FInputActionInstance& Instance)
{
	if (const UInputAction* StartedAction = Instance.GetSourceAction())
	{
		for (const auto& Pair : ActionMap)
		{
			if (Pair.Value == StartedAction)
			{
				OnActionStarted.Broadcast(Pair.Key);
				break;
			}
		}
	}
}

void UDynamicIMCComponent::HandleActionCompleted(const FInputActionInstance& Instance)
{
	if (const UInputAction* CompletedAction = Instance.GetSourceAction())
	{
		for (const auto& Pair : ActionMap)
		{
			if (Pair.Value == CompletedAction)
			{
				OnActionCompleted.Broadcast(Pair.Key);
				break;
			}
		}
	}
}
