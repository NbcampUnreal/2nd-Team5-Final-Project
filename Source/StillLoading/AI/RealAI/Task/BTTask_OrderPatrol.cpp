#include "BTTask_OrderPatrol.h"

#include "AI/RealAI/AISquadManager.h"
#include "AI/RealAI/Blackboardkeys.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_OrderPatrol::UBTTask_OrderPatrol()
{
	NodeName = TEXT("Order Squad To Patrol");
}

EBTNodeResult::Type UBTTask_OrderPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AAISquadManager* SquadManager = Cast<AAISquadManager>(BB->GetValueAsObject(BlackboardKeys::SquadManager));
	if (!SquadManager) return EBTNodeResult::Failed;

	SquadManager->OrderPatrol();
    
	return EBTNodeResult::Succeeded;
}