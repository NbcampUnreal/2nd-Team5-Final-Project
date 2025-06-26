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
    
	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsAtLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolTargetLocationKey;
};
