#pragma once

#include "CoreMinimal.h"
#include "SLMovementComponentBase.h"
#include "Character/Animation/SLAnimNotify.h"
#include "Character/Buffer/InputBufferComponent.h"
#include "Components/ActorComponent.h"
#include "SLMovementHandlerComponent.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UMovementHandlerComponent : public USLMovementComponentBase
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
	UFUNCTION()
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
	int MaxBlockCount = 3;

	// AttackState 카운트용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	int AttackStateCount = 0;

	// 카메라 입력 비 활성화 시 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraLagSpeed = 0.001f;

	// Lock on 용
	UPROPERTY()
	TObjectPtr<AActor> CameraFocusTarget;

	// TPS Camera View
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|View")
	bool bActivateTPSView = false;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnActionTriggered_Implementation(EInputActionType ActionType, FInputActionValue Value) override;
	virtual void OnActionStarted_Implementation(EInputActionType ActionType) override;
	virtual void OnActionCompleted_Implementation(EInputActionType ActionType) override;
	virtual void OnHitReceived_Implementation(AActor* Causer, float Damage, const FHitResult& HitResult, EHitAnimType AnimType) override;
	
	UFUNCTION()
	void RemoveInvulnerability() const;
	UFUNCTION()
	void HitDirection(AActor* Causer);
	UFUNCTION()
	void OnRadarDetectedActor(AActor* DetectedActor, float Distance);
	UFUNCTION()
	void FixCharacterVelocity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> EmpoweredShieldEffect;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveShieldEffectComponent;

	UPROPERTY()
	FVector2D MovementInputAxis = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Focus")
	float FocusMaxDistance = 800.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invulnerable")
	int32 InvulnerableDuration = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
	float WarpDistanceThreshold = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp")
	float ExecuteDistanceThreshold = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float DefaultArmLength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump")
	float JumpForwardImpulse = 500.0f;

private:
	void Attack();
	void BeginAttack();
	void Look(const FVector2D& Value);
	void Jump();

	// Zelda-Like
	void Move(const float AxisValue, const EInputActionType ActionType);

	// TPS
	void MoveTPS(const float AxisValue, const EInputActionType ActionType) const;
	void MoveForwardBackward(const float Value, const FRotator& YawRotation) const;
	void MoveLeftRight(const float Value, const FRotator& YawRotation) const;

	void SetViewMode(bool bIsTPS);
	void ToggleCameraState(bool bLock);
	bool ParryCheck();
	void PointMove();
	void DodgeLoco();
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

	float DesiredArmLength = 0;

	// 공중 공격 상태 저장 변수
	float DefaultGravityScale;
	float DefaultBrakingDecelerationFalling;
};
