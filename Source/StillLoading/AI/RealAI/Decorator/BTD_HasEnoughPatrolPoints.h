#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_HasEnoughPatrolPoints.generated.h"

UCLASS()
class STILLLOADING_API UBTD_HasEnoughPatrolPoints : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_HasEnoughPatrolPoints();

protected:
	UPROPERTY(EditAnywhere, Category = "AI|Patrol", meta = (ClampMin = "0"))
	int32 MinimumPatrolPoints = 2;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;
};
