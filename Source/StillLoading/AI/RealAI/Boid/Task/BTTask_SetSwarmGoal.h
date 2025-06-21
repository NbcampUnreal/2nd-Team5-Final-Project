#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetSwarmGoal.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_SetSwarmGoal : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTTask_SetSwarmGoal();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;
};
