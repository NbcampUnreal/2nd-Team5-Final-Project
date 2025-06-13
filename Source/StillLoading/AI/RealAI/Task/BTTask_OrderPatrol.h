#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_OrderPatrol.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_OrderPatrol : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_OrderPatrol();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
