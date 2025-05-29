#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckDistanceToTarget.generated.h"

struct FBlackboardKeySelector;

UCLASS()
class STILLLOADING_API UBTDecorator_CheckDistanceToTarget : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckDistanceToTarget();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	/** 거리 임계값 */
	UPROPERTY(EditAnywhere, Category = "Condition")
	float MaxDistance = 100.0f;

	/** 타겟 액터를 참조할 Blackboard Key */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};
