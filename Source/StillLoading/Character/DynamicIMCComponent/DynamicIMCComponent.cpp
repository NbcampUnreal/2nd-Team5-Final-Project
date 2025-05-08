#include "DynamicIMCComponent.h"

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
			if (const UInputAction* Action = Pair.Value)
			{
				InputComp->BindAction(Action, ETriggerEvent::Triggered, this, &UDynamicIMCComponent::HandleActionTriggered);
			}
		}
	}
}

void UDynamicIMCComponent::SetKeyForAction(EInputActionType ActionType, const FKey& NewKey)
{
	FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	EnumName.RemoveFromStart("EInputActionType::");
	UE_LOG(LogTemp, Display, TEXT("SetKeyForAction: %s → %s"), *EnumName, *NewKey.GetDisplayName().ToString());

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
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
				InputComp->BindAction(Action, ETriggerEvent::Started, this,
					&UDynamicIMCComponent::HandleActionTriggered);
			}
		}
	}

	Subsystem->AddMappingContext(CurrentIMC, 0);
}

void UDynamicIMCComponent::HandleActionTriggered(const FInputActionInstance& Instance)
 {
	if (const UInputAction* TriggeredAction = Instance.GetSourceAction())
	{
		for (const auto& Pair : ActionMap)
		{
			if (Pair.Value == TriggeredAction)
			{
				OnActionTriggered.Broadcast(Pair.Key);
				break;
			}
		}
	}
}
