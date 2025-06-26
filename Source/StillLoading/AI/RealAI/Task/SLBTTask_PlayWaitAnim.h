#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLBTTask_PlayWaitAnim.generated.h"

UCLASS()
class STILLLOADING_API USLBTTask_PlayWaitAnim : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLBTTask_PlayWaitAnim();

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
