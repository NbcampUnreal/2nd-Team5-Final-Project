#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "DynamicIMCComponent.generated.h"

UENUM(BlueprintType)
enum class EInputActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Jump UMETA(DisplayName = "Jump"),
	Move UMETA(DisplayName = "Move"),
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputTriggered, EInputActionType, ActionType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UDynamicIMCComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDynamicIMCComponent();

	void SetKeyForAction(EInputActionType ActionType, const FKey& NewKey);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditDefaultsOnly)
	TArray<FInputBindingData> ActionBindings;

	UPROPERTY(BlueprintAssignable)
	FOnInputTriggered OnActionTriggered;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UInputMappingContext* CurrentIMC;
	
	UFUNCTION()
	void HandleActionTriggered(const FInputActionInstance& ActionInstance);

	UPROPERTY()
	TMap<EInputActionType, UInputAction*> ActionMap;
};
