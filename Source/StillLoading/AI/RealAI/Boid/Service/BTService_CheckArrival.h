#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckArrival.generated.h"

UCLASS()
class STILLLOADING_API UBTService_CheckArrival : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckArrival();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
