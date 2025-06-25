#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RetreatFromTarget.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_RetreatFromTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RetreatFromTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RetreatLocation;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float MinimumDistance = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float MinRetreatDistance = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float MaxRetreatDistance = 500.0f;
};
