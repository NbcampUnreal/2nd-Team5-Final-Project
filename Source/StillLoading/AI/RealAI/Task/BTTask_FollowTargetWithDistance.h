#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FollowTargetWithDistance.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_FollowTargetWithDistance : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FollowTargetWithDistance();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Follow")
	float FollowDistance = 300.f;

	UPROPERTY(EditAnywhere, Category = "Follow")
	float UpdateInterval = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Follow")
	float AcceptanceRadius = 50.f;

private:
	float ElapsedTime = 0.f;
};
