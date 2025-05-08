#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLMovementHandlerComponent.generated.h"

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
	UFUNCTION()
	void BindIMCComponent();

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
	
	UFUNCTION()
	void Jump();
	UFUNCTION()
	void Move(const FVector& Direction);
	UFUNCTION()
	void Interact();
	UFUNCTION()
	void Attack();
	UFUNCTION()
	void PointMove();
	UFUNCTION()
	void ToggleWalk();
	UFUNCTION()
	void ToggleMenu();
};
