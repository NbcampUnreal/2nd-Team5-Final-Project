#pragma once

#include "CoreMinimal.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Components/ActorComponent.h"
#include "SLMovementHandlerComponent.generated.h"

class UCombatHandlerComponent;
class UBattleComponent;
class UAnimationMontageComponent;
class ASLPlayerCharacter;
class ASLPlayerCharacterBase;
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
	void HandleBufferedInput(ESkillType Action);
	void OnLanded(const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Rotation")
	float MinPitch = -80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Rotation")
	float MaxPitch = 80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity = 0.5f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnActionTriggered(EInputActionType ActionType, FInputActionValue Value);
	UFUNCTION()
	void OnActionStarted(EInputActionType ActionType);
	UFUNCTION()
	void OnActionCompleted(EInputActionType ActionType);
	UFUNCTION()
	void BindIMCComponent();

private:
	void Attack();
	void Look(const FVector2D& Value);
	void Jump();
	void Move(const float AxisValue, const EInputActionType ActionType);
	void Interact();
	void PointMove();
	void ToggleWalk(const bool bNewWalking);
	void ToggleMenu();
	void Block(const bool bIsBlocking);
	void ApplyAttackState(const FName& SectionName, bool bIsFalling);
	
	//UPROPERTY()
	float InputBufferDuration = 0.3f;

	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UAnimationMontageComponent> CachedMontageComponent;
	UPROPERTY()
	TObjectPtr<UBattleComponent> CachedBattleComponent;
	UPROPERTY()
	TObjectPtr<UCombatHandlerComponent> CachedCombatComponent;

	int32 CurrentIndex = 0; // Test용
};
