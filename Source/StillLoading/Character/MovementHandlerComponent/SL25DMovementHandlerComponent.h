#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "SL2DMovementHandlerComponent.h"
#include "Components/ActorComponent.h"
#include "SL25DMovementHandlerComponent.generated.h"


class UNiagaraSystem;
class USLSoundSubsystem;
class UCollisionRadarComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USL25DMovementHandlerComponent : public USLMovementComponentBase
{
	GENERATED_BODY()

public:
	USL25DMovementHandlerComponent();

	UFUNCTION()
	void OnAttackStageFinished(ECharacterMontageState AttackStage);
	UFUNCTION()
	void BeginBuff();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Effects")
	TObjectPtr<UNiagaraSystem> MoveTargetEffect;

	// 캐릭터가 마우스 커서를 바라보게 할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rotation")
	bool bShouldFaceMouse = false;

	// 초당 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rotation")
	float RotationSpeed = 10.0f;

	// 캐릭터 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Movement")
	float MovementSpeed = 600.0f;

	// 목표 지점에 도착했다고 판단할 거리 (반경)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Movement", meta = (ClampMin = "0.0"))
	float AcceptanceRadius = 25.0f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnActionTriggered_Implementation(EInputActionType ActionType, FInputActionValue Value) override;
	virtual void OnActionStarted_Implementation(EInputActionType ActionType) override;
	virtual void OnActionCompleted_Implementation(EInputActionType ActionType) override;
	virtual void OnHitReceived_Implementation(AActor* Causer, float Damage, const FHitResult& HitResult, EHitAnimType AnimType) override;

	UFUNCTION(BlueprintCallable, Category = "Movement|Rotation")
	void StartFacingMouse();
	UFUNCTION(BlueprintCallable, Category = "Movement|Rotation")
	void StopFacingMouse();

	UFUNCTION()
	void OnDelayedAction();
	UFUNCTION()
	void RemoveInvulnerability() const;
	UFUNCTION()
	void HitDirection(AActor* Causer);
	UFUNCTION()
	void RotateToHitCauser(const AActor* Causer, FRotator& TargetRotation, bool& bIsHitFromBack);
	UFUNCTION()
	void DodgeLoco();
	UFUNCTION()
	void ToggleMenu();
	UFUNCTION()
	void Block(bool bIsBlocking);
	UFUNCTION()
	void Attack();
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartMoveToMouseCursorLocation();
	UFUNCTION()
	void ApplyAttackState(const FName& SectionName, bool bIsFalling);
	UFUNCTION()
	void Move(float AxisValue, EInputActionType ActionType);
	UFUNCTION()
	void FaceMouseCursorInstantly() const;

	UPROPERTY()
	FTimerHandle ReactionResetTimerHandle;
	UPROPERTY()
	FTimerHandle InvulnerabilityTimerHandle;
	UPROPERTY()
	FTimerHandle DelayTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Camera Focus")
	int32 InvulnerableDuration = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float ForwardDot = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float RightDot = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Parry")
	float ParryDuration = 0.2f;
	
	UPROPERTY()
	int BlockCount = 0;
	UPROPERTY()
	float LastBlockTime = 0.f;

	// 막기용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Block")
	int MaxBlockCount = 5;

private:
	void HandleRotation(float DeltaTime);
	void MoveToTarget(float DeltaTime);

	FTimerHandle FacingMouseTimerHandle;

	FVector TargetMoveLocation;
	bool bIsMovingToTarget = false;
};
