#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementHandlerComponent.generated.h"

enum class EInputActionType : uint8;

class UInputAction;
class UDynamicIMCComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UMovementHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMovementHandlerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnActionTriggered(EInputActionType ActionType);

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UFUNCTION()
	void BindIMCComponent();

	UFUNCTION()
	void Jump();
};
