#pragma once

#include "CoreMinimal.h"
#include "Character/MovementHandlerComponent/SLMovementHandlerComponent.h"
#include "Components/ActorComponent.h"
#include "InputBufferComponent.generated.h"

USTRUCT(BlueprintType)
struct FBufferedInput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInputActionType Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Timestamp;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInputBufferComponent();

	UFUNCTION()
	void AddBufferedInput(EInputActionType Action);
	UFUNCTION()
	void ProcessBufferedInputs();
	UFUNCTION()
	void ClearBuffer();
	UFUNCTION()
	void OnIMCActionStarted(EInputActionType ActionType);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void ExecuteInput(EInputActionType Action);
	UFUNCTION()
	bool CanConsumeInput() const;

	UPROPERTY()
	TArray<FBufferedInput> InputBuffer;
	UPROPERTY()
	float BufferDuration = 1.0f;
	UPROPERTY()
	bool bBufferActive = false;
};
