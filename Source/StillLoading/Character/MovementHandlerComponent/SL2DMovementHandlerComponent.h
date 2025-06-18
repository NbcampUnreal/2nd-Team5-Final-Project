#pragma once

#include "CoreMinimal.h"
#include "SLMovementComponentBase.h"
#include "Components/ActorComponent.h"
#include "SL2DMovementHandlerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USL2DMovementHandlerComponent : public USLMovementComponentBase
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
	virtual void OnActionTriggered_Implementation(EInputActionType ActionType, FInputActionValue Value) override;
	virtual void OnActionStarted_Implementation(EInputActionType ActionType) override;
	virtual void OnActionCompleted_Implementation(EInputActionType ActionType) override;
	virtual void OnHitReceived_Implementation(AActor* Causer, float Damage, const FHitResult& HitResult, EHitAnimType AnimType) override;
	
private:
	void Move(const float AxisValue, const EInputActionType ActionType);
	void Attack();
	void ApplyAttackState(const FName& SectionName, bool bIsFalling);
};