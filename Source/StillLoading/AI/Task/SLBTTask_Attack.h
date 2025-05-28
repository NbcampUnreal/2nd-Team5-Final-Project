#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLBTTask_Attack.generated.h"

UCLASS()
class STILLLOADING_API USLBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLBTTask_Attack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
