#pragma once

#include "CoreMinimal.h"
#include "Character/Animation/SLAnimNotify.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
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
	UFUNCTION()
	void StartKnockback(float Speed, float Duration);

	// Mouse 제어용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Rotation")
	float MinPitch = -80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Rotation")
	float MaxPitch = 80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity = 0.5f;

	// 피격시 BlendSpace 용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float ForwardDot = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float RightDot = 0.0f;

	// 패링용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Parry")
	float ParryDuration = 0.2f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void OnActionTriggered(EInputActionType ActionType, FInputActionValue Value);
	UFUNCTION()
	void OnActionStarted(EInputActionType ActionType);
	UFUNCTION()
	void OnActionCompleted(EInputActionType ActionType);
	UFUNCTION()
	void BindIMCComponent();
	UFUNCTION()
	void OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EAttackAnimType AnimType);
	UFUNCTION()
	void HitDirection(AActor* Causer);

	UPROPERTY()
	FVector2D MovementInputAxis = FVector2D::ZeroVector;

private:
	void Attack();
	void Look(const FVector2D& Value);
	void Jump();
	void Move(const float AxisValue, const EInputActionType ActionType);
	void Interact();
	void PointMove();
	void ToggleWalk(const bool bNewWalking);
	void ToggleMenu();
	void Dodge();
	void Airborne();
	void AirUp();
	void AirDown();
	void Execution();
	void Block(const bool bIsBlocking);
	void RotateToHitCauser(const AActor* Causer, FRotator &TargetRotation, bool &bIsHitFromBack);
	void ApplyAttackState(const FName& SectionName, bool bIsFalling);

	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UAnimationMontageComponent> CachedMontageComponent;
	UPROPERTY()
	TObjectPtr<UCombatHandlerComponent> CachedCombatComponent;
	UPROPERTY()
	TObjectPtr<UBattleComponent> CachedBattleComponent;
	UPROPERTY()
	FTimerHandle ReactionResetTimerHandle;

	// 넉백용
	UPROPERTY()
	bool bDoKnockback = false;
	UPROPERTY()
	float KnockbackTimer = 0.0f;
	UPROPERTY()
	float KnockbackSpeed = 1200.f;
	UPROPERTY()
	float KnockbackTime = 0.3f;

	// Block 용
	UPROPERTY()
	int BlockCount = 0;
	UPROPERTY()
	float LastBlockTime = 0.f;
};
