#include "InputBufferComponent.h"

#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"

UInputBufferComponent::UInputBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType,
										  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessBufferedInputs();
}

void UInputBufferComponent::AddBufferedInput(EInputActionType Action)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	InputBuffer.Add({ Action, CurrentTime });
}

void UInputBufferComponent::ProcessBufferedInputs()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	InputBuffer = InputBuffer.FilterByPredicate([&](const FBufferedInput& Input)
	{
		return (CurrentTime - Input.Timestamp) <= BufferDuration;
	});

	if (InputBuffer.Num() > 0)
	{
		const EInputActionType NextInput = InputBuffer[0].Action;

		if (CanConsumeInput())
		{
			ExecuteInput(NextInput);
			InputBuffer.RemoveAt(0);
		}
	}
}

void UInputBufferComponent::ClearBuffer()
{
	InputBuffer.Empty();
	SetComponentTickEnabled(false);
}

void UInputBufferComponent::OnIMCActionStarted(EInputActionType ActionType)
{
	AddBufferedInput(ActionType);
}

bool UInputBufferComponent::CanConsumeInput() const
{
	//TODO::동작 조건 정의 필요

	// 공격 연속 3회 -> 공격1 공격2 공격3
	// 공격1 방어 공격1

	
	
	return true;
}

void UInputBufferComponent::ExecuteInput(EInputActionType Action)
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteInput: %d on Actor %s"), (int32)Action, *Owner->GetName());
		if (UMovementHandlerComponent* MovementComp = GetOwner()->FindComponentByClass<UMovementHandlerComponent>())
		{
			MovementComp->Attack();
		}
	}
}

