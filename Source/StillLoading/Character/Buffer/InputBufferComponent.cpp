#include "InputBufferComponent.h"

#include "Character/SLCharacter.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"

UInputBufferComponent::UInputBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ASLCharacter>(GetOwner());
	}
}

void UInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType,
										  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessBufferedInputs();
}

void UInputBufferComponent::AddBufferedInput(EInputActionType Action)
{
	if (InputBuffer.Num() > MaxInputBufferCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputBufferComponent: Input buffer full."));
		return;
	}
	
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	LastInputTime = GetWorld()->GetTimeSeconds();
	InputBuffer.Add({ Action, CurrentTime });

	UE_LOG(LogTemp, Warning, TEXT("InputBufferComponent: Input [%s] array [%d]."), *UEnum::GetValueAsString(Action), InputBuffer.Num());
}

void UInputBufferComponent::ProcessBufferedInputs()
{
	const UWorld* World = GetWorld();
	if (!World) return;

	if (LastInputTime < 0.0f)
		return;

	const float CurrentTime = World->GetTimeSeconds();

	if ((CurrentTime - LastInputTime) > BufferDuration)
	{
		if (UCombatHandlerComponent* CombatComp = GetOwner()->FindComponentByClass<UCombatHandlerComponent>())
		{
			CombatComp->ResetCombo();
			UE_LOG(LogTemp, Warning, TEXT("Combo Reset!"));
		}

		LastInputTime = -1.0f;
	}

	InputBuffer = InputBuffer.FilterByPredicate([&](const FBufferedInput& Input)
	{
		return (CurrentTime - Input.Timestamp) <= BufferDuration;
	});

	if (!InputBuffer.IsEmpty())
	{
		const EInputActionType NextInput = InputBuffer[0].Action;

		if (CanConsumeInput(NextInput))
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

bool UInputBufferComponent::CanConsumeInput(EInputActionType NextInput) const
{
	//TODO::동작 조건 정의 필요
	// 공격 연속 3회 -> 공격1 공격2 공격3
	// 공격1 방어 공격1

	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_AttackBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMovementHandlerComponent: Attack Blocked"));
		return false;
	}
	
	return true;
}

void UInputBufferComponent::ExecuteInput(EInputActionType Action)
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		if (UMovementHandlerComponent* MovementComp = GetOwner()->FindComponentByClass<UMovementHandlerComponent>())
		{
			MovementComp->HandleBufferedInput(Action);
		}
	}
}

