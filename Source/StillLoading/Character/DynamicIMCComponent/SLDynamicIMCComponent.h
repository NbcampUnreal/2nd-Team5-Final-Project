#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "SLDynamicIMCComponent.generated.h"

UENUM(BlueprintType)
enum class EInputActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Jump UMETA(DisplayName = "Jump"),
	Look UMETA(DisplayName = "Look"),
	MoveUp UMETA(DisplayName = "MoveUp"),
	MoveDown UMETA(DisplayName = "MoveDown"),
	MoveLeft UMETA(DisplayName = "MoveLeft"),
	MoveRight UMETA(DisplayName = "MoveRight"),
	Interaction UMETA(DisplayName = "Interaction"),
	Attack UMETA(DisplayName = "Attack"),
	PointMove UMETA(DisplayName = "PointMove"),
	Walk UMETA(DisplayName = "Walk"),
	Menu UMETA(DisplayName = "Menu")
};

USTRUCT(BlueprintType)
struct FInputBindingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInputActionType ActionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* Action;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputTriggered, EInputActionType, ActionType, FInputActionValue, InputValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputStarted, EInputActionType, ActionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputStopped, EInputActionType, ActionType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UDynamicIMCComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDynamicIMCComponent();

	UFUNCTION()
	void SetKeyForAction(EInputActionType ActionType, const FKey& NewKey);
	UFUNCTION()
	void BindInputForAction(const UInputAction* Action, EInputActionType ActionType, UEnhancedInputComponent* InputComp);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditDefaultsOnly)
	TArray<FInputBindingData> ActionBindings;

	UPROPERTY(BlueprintAssignable)
	FOnInputTriggered OnActionTriggered;

	UPROPERTY(BlueprintAssignable)
	FOnInputStarted OnActionStarted;

	UPROPERTY(BlueprintAssignable)
	FOnInputStopped OnActionCompleted;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleActionTriggered(const FInputActionInstance& ActionInstance);
	UFUNCTION()
	void HandleActionStarted(const FInputActionInstance& Instance);
	UFUNCTION()
	void HandleActionCompleted(const FInputActionInstance& Instance);

	UFUNCTION()
	void BindDefaultSetting();

	UPROPERTY()
	TMap<EInputActionType, UInputAction*> ActionMap;
	UPROPERTY()
	UInputMappingContext* CurrentIMC;
};
