#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SetTargetLocation.generated.h"

UCLASS()
class STILLLOADING_API UBTDecorator_SetTargetLocation : public UBTDecorator
{
	GENERATED_BODY()

	UBTDecorator_SetTargetLocation();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetLocationKey;
};
