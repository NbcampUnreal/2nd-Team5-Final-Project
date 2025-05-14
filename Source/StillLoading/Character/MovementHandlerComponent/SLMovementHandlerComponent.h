#pragma once

#include "CoreMinimal.h"
#include "Character/CameraManagerComponent/CameraManagerComponent.h"
#include "Components/ActorComponent.h"
#include "SLMovementHandlerComponent.generated.h"

class UCombatHandlerComponent;
class UBattleComponent;
class UAnimationMontageComponent;
class ASLCharacter;
class ASLBaseCharacter;
struct FInputActionValue;
enum class EInputActionType : uint8;

class UInputAction;
class UDynamicIMCComponent;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_Cinematic UMETA(DisplayName = "Cinematic"),
	ECS_Moving UMETA(DisplayName = "Moving"),
	ECS_Jumping UMETA(DisplayName = "Jumping"),
	ECS_Falling UMETA(DisplayName = "Falling"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),
	ECS_Dodging UMETA(DisplayName = "Dodging"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	ECS_Dead UMETA(DisplayName = "Dead"),
	ECS_InputLocked UMETA(DisplayName = "Input Locked")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UMovementHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMovementHandlerComponent();
	
	// 애니매이션 노티 확인용
	UFUNCTION()
	void OnAttackStageFinished(ECharacterMontageState AttackStage);
	// 버퍼 출력 처리용
	UFUNCTION()
	void HandleBufferedInput(EInputActionType Action);

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
	void Attack();
	UFUNCTION()
	void Look(const FVector2D& Value);
	UFUNCTION()
	void Jump();
	UFUNCTION()
	void Move(const float AxisValue, const EInputActionType ActionType);
	UFUNCTION()
	void Interact();
	UFUNCTION()
	void PointMove();
	UFUNCTION()
	void ToggleWalk(const bool bNewWalking);
	UFUNCTION()
	void ToggleMenu();
	UFUNCTION()
	void Block(const bool bIsBlocking);
	UFUNCTION()
	EGameCameraType GetCurrentCameraType() const;

	UPROPERTY()
	float InputBufferDuration = 0.3f;

	UPROPERTY()
	TObjectPtr<ASLCharacter> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UAnimationMontageComponent> CachedMontageComponent;
	UPROPERTY()
	TObjectPtr<UBattleComponent> CachedBattleComponent;
	UPROPERTY()
	TObjectPtr<UCombatHandlerComponent> CachedCombatComponent;

	int32 CurrentIndex = 0; // Test용
};
