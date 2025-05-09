#include "SLDynamicIMCComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/SLBaseCharacter.h"

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

		if (ASLBaseCharacter* OwnerChar = Cast<ASLBaseCharacter>(GetOwner()))
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
	case EInputActionType::MoveUp:
	case EInputActionType::MoveDown:
	case EInputActionType::MoveLeft:
	case EInputActionType::MoveRight:
	case EInputActionType::Look:
		InputComp->BindAction(Action, ETriggerEvent::Triggered, this, &UDynamicIMCComponent::HandleActionTriggered);
		break;

	case EInputActionType::Walk:
	case EInputActionType::Jump:
	case EInputActionType::Interaction:
	case EInputActionType::Attack:
	case EInputActionType::PointMove:
	case EInputActionType::Menu:
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
