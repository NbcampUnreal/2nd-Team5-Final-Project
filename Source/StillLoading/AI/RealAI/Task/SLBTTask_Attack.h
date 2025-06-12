#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLBTTask_Attack.generated.h"

enum class EAttackAnimType : uint8;

UCLASS()
class STILLLOADING_API USLBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USLBTTask_Attack();

	UPROPERTY(EditAnywhere, Category = "Cooldown")
	FName LastAttackTimeKey = "LastAttackTime";

	UPROPERTY(EditAnywhere, Category = "AttackType")
	EAttackAnimType AttackType;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
