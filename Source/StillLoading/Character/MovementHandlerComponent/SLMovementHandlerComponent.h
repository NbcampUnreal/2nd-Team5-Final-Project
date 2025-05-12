#pragma once

#include "CoreMinimal.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Components/ActorComponent.h"
#include "SLMovementHandlerComponent.generated.h"

class ASLBaseCharacter;
struct FInputActionValue;
enum class EInputActionType : uint8;

class UInputAction;
class UDynamicIMCComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UMovementHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMovementHandlerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Rotation")
	float MinPitch = -80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Rotation")
	float MaxPitch = 80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity = 0.5f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnActionTriggered(EInputActionType ActionType, FInputActionValue Value);
	UFUNCTION()
	void OnActionStarted(EInputActionType ActionType);
	UFUNCTION()
	void OnActionCompleted(EInputActionType ActionType);
	UFUNCTION()
	void BindIMCComponent();

private:
	UFUNCTION()
	void Look(const FVector2D& Value);
	UFUNCTION()
	void Jump();
	UFUNCTION()
	void Move(const float AxisValue, const EInputActionType ActionType);
	UFUNCTION()
	void Interact();
	UFUNCTION()
	void Attack();
	UFUNCTION()
	void PointMove();
	UFUNCTION()
	void ToggleWalk(const bool bNewWalking);
	UFUNCTION()
	void ToggleMenu();
	UFUNCTION()
	EGameCameraType GetCurrentCameraType() const;

	UPROPERTY()
	TObjectPtr<ASLBaseCharacter> OwnerCharacter;

	int32 CurrentIndex = 0; // Test용
};
