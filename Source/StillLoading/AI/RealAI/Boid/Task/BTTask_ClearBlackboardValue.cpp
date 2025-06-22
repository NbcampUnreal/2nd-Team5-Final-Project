#include "BTTask_ClearBlackboardValue.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackboardValue::UBTTask_ClearBlackboardValue()
{
	NodeName = TEXT("Clear Blackboard Value (Custom)");
}

EBTNodeResult::Type UBTTask_ClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	BlackboardComp->ClearValue(BlackboardKeyToClear.SelectedKeyName);
	
	return EBTNodeResult::Succeeded;
}