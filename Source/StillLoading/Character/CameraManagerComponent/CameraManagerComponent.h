#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraManagerComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum class EGameCameraType : uint8
{
	EGCT_Default UMETA(DisplayName = "Default"),
	EGCT_Battle UMETA(DisplayName = "Battle"),
	EGCT_TopDown UMETA(DisplayName = "TopDown"),
	EGCT_SideView UMETA(DisplayName = "SideView"),

	EGCT_MAX UMETA(Hidden) // 범위 체크용
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UCameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SwitchCamera(const EGameCameraType NewType);
	UFUNCTION()
	void SetCameraRefs(UCameraComponent* InDefault, UCameraComponent* InBattle, UCameraComponent* InTopDown, UCameraComponent* InSideView);
	
	void SetRetroCameraRotation();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void UpdateView();
	UFUNCTION()
	UCameraComponent* GetCameraByType(EGameCameraType Type) const;

	UPROPERTY()
	TObjectPtr<UCameraComponent> DefaultCamera;
	UPROPERTY()
	TObjectPtr<UCameraComponent> BattleCamera;
	UPROPERTY()
	TObjectPtr<UCameraComponent> TopDownCamera;
	UPROPERTY()
	TObjectPtr<UCameraComponent> SideViewCamera;
	UPROPERTY()
	TObjectPtr<UCameraComponent> CurrentCamera;

	UPROPERTY()
	EGameCameraType CurrentType = EGameCameraType::EGCT_Default;

public:
	UFUNCTION()
	FORCEINLINE_DEBUGGABLE EGameCameraType GetCurrentCameraType() const { return CurrentType; }
};
