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
	float SearchRadius = 800.f;

	UPROPERTY(EditAnywhere, Category = "Wander")
	int32 MaxAttempts = 10;

	UPROPERTY(EditAnywhere, Category = "Wander")
	float MinDistanceFromOthers = 200.f;
};
