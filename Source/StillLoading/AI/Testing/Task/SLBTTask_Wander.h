#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLBTTask_Wander.generated.h"

UCLASS()
class STILLLOADING_API USLBTTask_Wander : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLBTTask_Wander();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Wander")
	float WanderRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DestinationKey;
};
