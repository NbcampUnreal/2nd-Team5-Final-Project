#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetNextPatrolPoint.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_GetNextPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GetNextPatrolPoint();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolIndexKey;
};
