// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SL2DMovementHandlerComponent.generated.h"

class ASLPlayerCharacter;
class UCombatHandlerComponent;
class UBattleComponent;
class UAnimationMontageComponent;
enum class EInputActionType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USL2DMovementHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USL2DMovementHandlerComponent();

	// 애니매이션 노티 확인용
	UFUNCTION()
	void OnAttackStageFinished(ECharacterMontageState AttackStage);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void BindIMCComponent();
	UFUNCTION()
	void OnActionTriggered(EInputActionType ActionType, FInputActionValue Value);
	UFUNCTION()
	void OnActionStarted(EInputActionType ActionType);
	UFUNCTION()
	void OnActionCompleted(EInputActionType ActionType);
	UFUNCTION()
	void Move(const float AxisValue, const EInputActionType ActionType);

	void MoveByPixelUnit(const FVector& Direction, float AxisValue);
	void StartPixelMovement(const FVector& Direction);
	
	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UAnimationMontageComponent> CachedMontageComponent;
	UPROPERTY()
	TObjectPtr<UBattleComponent> CachedBattleComponent;
	UPROPERTY()
	TObjectPtr<UCombatHandlerComponent> CachedCombatComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialParameterCollection> PixelizationMPC;
	
	// 픽셀 이동 관련 변수
	bool bIsMoving = false;
	FVector CurrentTargetLocation = FVector::ZeroVector;
	FVector MoveDirection = FVector::ZeroVector;
	float PixelSize = 0.0f;
};