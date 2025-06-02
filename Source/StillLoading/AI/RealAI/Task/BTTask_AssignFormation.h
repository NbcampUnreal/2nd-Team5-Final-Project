#pragma once

#include "CoreMinimal.h"
#include "AI/RealAI/FormationComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AssignFormation.generated.h"

UCLASS()
class STILLLOADING_API UBTTask_AssignFormation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AssignFormation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Formation")
	EFormationType FormationType;

	UPROPERTY(EditAnywhere, Category = "Formation")
	float Spacing = 200.f;
};
