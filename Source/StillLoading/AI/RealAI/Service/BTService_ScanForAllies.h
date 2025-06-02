#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ScanForAllies.generated.h"

UCLASS()
class STILLLOADING_API UBTService_ScanForAllies : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_ScanForAllies();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Scan")
	float ScanRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector AllyCountKey;
};
