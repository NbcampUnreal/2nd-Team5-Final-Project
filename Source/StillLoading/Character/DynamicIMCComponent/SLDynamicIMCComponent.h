#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "SLDynamicIMCComponent.generated.h"

UENUM(BlueprintType)
enum class EInputActionType : uint8
{
	EIAT_None UMETA(DisplayName = "None"),
	EIAT_MoveUp UMETA(DisplayName = "MoveUp"),
	EIAT_MoveDown UMETA(DisplayName = "MoveDown"),
	EIAT_MoveLeft UMETA(DisplayName = "MoveLeft"),
	EIAT_MoveRight UMETA(DisplayName = "MoveRight"),
	EIAT_CameraUnlock UMETA(DisplayName = "CameraUnlock"),
	EIAT_Jump UMETA(DisplayName = "Jump"),
	EIAT_Look UMETA(DisplayName = "Look"),
	EIAT_Attack UMETA(DisplayName = "Attack"),
	EIAT_Block UMETA(DisplayName = "Block"),
	EIAT_Interaction UMETA(DisplayName = "Interaction"),
	EIAT_Walk UMETA(DisplayName = "Walk"),
	EIAT_PointMove UMETA(DisplayName = "PointMove"),
	EIAT_Menu UMETA(DisplayName = "Menu"),
	EIAT_LockObject UMETA(DisplayName = "LockObject"),
	EIAT_Special UMETA(DisplayName = "Special"),
	EIAT_Max UMETA(DisplayName = "Max")
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

	UFUNCTION(BlueprintCallable, Category = "Input")
	void BindDefaultSetting();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void BindCurrentSetting(UInputMappingContext* NewIMC, int32 Priority);
	
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


	UPROPERTY()
	TMap<EInputActionType, UInputAction*> ActionMap;
	UPROPERTY()
	TObjectPtr<UInputMappingContext> CurrentIMC;
};
