#pragma once

#include "CoreMinimal.h"
#include "Character/Animation/SLAnimNotify.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "Components/ActorComponent.h"
#include "SLMovementHandlerComponent.generated.h"

class UCollisionRadarComponent;
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
	// Lock on 용
	UFUNCTION()
	void DisableLock();
	// 버프용
	UFUNCTION()
	void BeginBuff();
	UFUNCTION()
	void ToggleCameraZoom(bool bIsZoomedOut, float ZoomedOutArmLength = 500.f);

	// Mouse 제어용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Rotation")
	float MinPitch = -80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Rotation")
	float MaxPitch = 80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraZoomInterpSpeed = 5.0f;

	// 피격시 BlendSpace 용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float ForwardDot = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float RightDot = 0.0f;

	// 패링용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Parry")
	float ParryDuration = 0.2f;

	// 막기용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block")
	int MaxBlockCount = 5;

	// AttackState 카운트용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	int AttackStateCount = 0;

	// Lock on 용
	UPROPERTY()
	TObjectPtr<AActor> CameraFocusTarget;

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
	void RemoveInvulnerability() const;
	UFUNCTION()
	void OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EAttackAnimType AnimType);
	UFUNCTION()
	void HitDirection(AActor* Causer);
	UFUNCTION()
	void OnRadarDetectedActor(AActor* DetectedActor, float Distance);
	UFUNCTION()
	void FixCharacterVelocity();

	UPROPERTY()
	FVector2D MovementInputAxis = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Focus")
	float FocusMaxDistance = 800.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invulnerable")
	int32 InvulnerableDuration = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
	float WarpDistanceThreshold = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
	float ExecuteDistanceThreshold = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float DefaultArmLength = 300.f;

private:
	void Attack();
	void BeginAttack();
	void Look(const FVector2D& Value);
	void Jump();
	void Move(const float AxisValue, const EInputActionType ActionType);
	void Interact();
	void PointMove();
	void DodgeLoco();
	void ToggleMenu();
	void ToggleLockState();
	void RotateCameraToTarget(const AActor* Target, float DeltaTime);
	void SpawnSword();
	void Airborne();
	void AirUp();
	void AirDown();
	void Execution();
	void Blast(const EItemType ItemType);
	void Block(const bool bIsBlocking);
	void RotateToHitCauser(const AActor* Causer, FRotator &TargetRotation, bool &bIsHitFromBack);
	void ApplyAttackState(const FName& SectionName, bool bIsFalling);

	void Test();

	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UAnimationMontageComponent> CachedMontageComponent;
	UPROPERTY()
	TObjectPtr<UCombatHandlerComponent> CachedCombatComponent;
	UPROPERTY()
	TObjectPtr<UBattleComponent> CachedBattleComponent;
	UPROPERTY()
	TObjectPtr<UCollisionRadarComponent> CachedRadarComponent;
	UPROPERTY()
	FTimerHandle ReactionResetTimerHandle;
	UPROPERTY()
	FTimerHandle InvulnerabilityTimerHandle;

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
	UFUNCTION()
	void OnDelayedAction();
	UPROPERTY()
	int BlockCount = 0;
	UPROPERTY()
	float LastBlockTime = 0.f;
	UPROPERTY()
	FTimerHandle DelayTimerHandle;

	// Begin Attack 용
	UPROPERTY()
	bool bDidBeginAttack = false;

	float DesiredArmLength;
	
};
