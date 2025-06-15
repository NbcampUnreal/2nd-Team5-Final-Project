#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "SL2DMovementHandlerComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "Character/DataAsset/InputComboRow.h"
#include "Components/ActorComponent.h"
#include "SL25DMovementHandlerComponent.generated.h"


class UCollisionRadarComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USL25DMovementHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USL25DMovementHandlerComponent();

	UFUNCTION()
	void OnAttackStageFinished(ECharacterMontageState AttackStage);
	UFUNCTION()
	void BeginBuff();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Movement|Rotation")
	void StartFacingMouse();

	/** 호출되면, Tick에서 마우스를 향한 회전을 중지합니다. */
	UFUNCTION(BlueprintCallable, Category = "Movement|Rotation")
	void StopFacingMouse();

	UFUNCTION()
	void OnRadarDetectedActor(AActor* DetectedActor, float Distance);
	UFUNCTION()
	void BindIMCComponent();
	UFUNCTION()
	void OnActionTriggered(EInputActionType ActionType, FInputActionValue Value);
	UFUNCTION()
	void OnActionStarted(EInputActionType ActionType);
	UFUNCTION()
	void OnActionCompleted(EInputActionType ActionType);
	UFUNCTION()
	void OnHitReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EHitAnimType AnimType);
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
	void Interact();
	UFUNCTION()
	void Attack();
	UFUNCTION()
	void ApplyAttackState(const FName& SectionName, bool bIsFalling);
	UFUNCTION()
	void Move(float AxisValue, EInputActionType ActionType);
	UFUNCTION()
	void FaceToMouse();

	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> OwnerCharacter;
	
	UPROPERTY()
	TSoftObjectPtr<UAnimationMontageComponent> CachedMontageComponent;
	UPROPERTY()
	TSoftObjectPtr<UCombatHandlerComponent> CachedCombatComponent;
	UPROPERTY()
	TSoftObjectPtr<UBattleComponent> CachedBattleComponent;
	UPROPERTY()
	TSoftObjectPtr<UCollisionRadarComponent> CachedRadarComponent;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedSkeletalMesh;

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

	bool bShouldFaceMouse = false;

	UPROPERTY(EditAnywhere, Category = "Movement|Rotation")
	float RotationSpeed = 15.0f;
	
	UPROPERTY()
	int BlockCount = 0;
	UPROPERTY()
	float LastBlockTime = 0.f;
};
