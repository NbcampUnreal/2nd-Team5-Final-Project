// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SL2DMovementHandlerComponent.generated.h"

class ASLBaseReactiveObject;
class ISLInteractableBase;
class UCollisionRadarComponent;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	int AttackStateCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	int AttackStateCountLimit = 25;
	
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
	void OnRadarDetectedActor(AActor* DetectedActor, float Distance);
	
	void Move(const float AxisValue, const EInputActionType ActionType);
	void Attack();
	void ApplyAttackState(const FName& SectionName, bool bIsFalling);
	void Interaction();
	
	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UAnimationMontageComponent> CachedMontageComponent;
	UPROPERTY()
	TObjectPtr<UBattleComponent> CachedBattleComponent;
	UPROPERTY()
	TObjectPtr<UCombatHandlerComponent> CachedCombatComponent;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedSkeletalMesh;
	UPROPERTY()
	TObjectPtr<UCollisionRadarComponent> CachedRadarComponent;
	UPROPERTY()
	TWeakObjectPtr<ASLBaseReactiveObject> DetectedReactiveObject;
};