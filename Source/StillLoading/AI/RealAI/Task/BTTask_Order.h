#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Order.generated.h"

enum class EOrderType : uint8;

UCLASS()
class STILLLOADING_API UBTTask_Order : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Order();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "AttackType")
	EOrderType AttackType;
};
