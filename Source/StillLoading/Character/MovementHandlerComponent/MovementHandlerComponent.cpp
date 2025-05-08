#include "MovementHandlerComponent.h"

#include "Character/SLBaseCharacter.h"
#include "Character/DynamicIMCComponent/DynamicIMCComponent.h"

UMovementHandlerComponent::UMovementHandlerComponent(): OwnerCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovementHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	BindIMCComponent();
}

void UMovementHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMovementHandlerComponent::OnActionTriggered(EInputActionType ActionType)
{
	FString EnumName = StaticEnum<EInputActionType>()->GetNameStringByValue(static_cast<int64>(ActionType));
	EnumName.RemoveFromStart("EInputActionType::");

	UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent::OnActionTriggered → %s"), *EnumName);
	
	switch (ActionType)
	{
	case EInputActionType::Jump:
		Jump();
		break;

	default:
		break;
	}
}

void UMovementHandlerComponent::BindIMCComponent()
{
	OwnerCharacter = Cast<ASLBaseCharacter>(GetOwner());
	if (auto* IMC = OwnerCharacter->FindComponentByClass<UDynamicIMCComponent>())
	{
		IMC->OnActionTriggered.AddDynamic(this, &UMovementHandlerComponent::OnActionTriggered);
	}
}

void UMovementHandlerComponent::Jump()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump"));
	if (OwnerCharacter) OwnerCharacter->Jump();
}

