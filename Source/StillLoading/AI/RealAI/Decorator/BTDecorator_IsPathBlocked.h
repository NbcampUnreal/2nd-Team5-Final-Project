#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsPathBlocked.generated.h"

UCLASS()
class STILLLOADING_API UBTDecorator_IsPathBlocked : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsPathBlocked();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
