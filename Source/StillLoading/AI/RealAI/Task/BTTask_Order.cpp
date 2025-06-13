#include "BTTask_Order.h"

#include "AI/RealAI/AISquadManager.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Order::UBTTask_Order(): AttackType()
{
	NodeName = "Order";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_Order::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AAISquadManager* SquadManager = Cast<AAISquadManager>(BB->GetValueAsObject(TEXT("SquadManager")));
	if (!SquadManager) return EBTNodeResult::Failed;

	SquadManager->Order(AttackType);
	return EBTNodeResult::Succeeded;
}
