#include "BTTask_Order.h"

#include "AI/RealAI/FormationComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Order::UBTTask_Order()
{
	NodeName = "Order";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_Order::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* Leader = Cast<AActor>(BB->GetValueAsObject(FName("Leader")));
	if (!Leader) return EBTNodeResult::Failed;

	UFormationComponent* FormationComp = Leader->FindComponentByClass<UFormationComponent>();
	if (!FormationComp) return EBTNodeResult::Failed;

	FormationComp->Order(AttackType);

	return EBTNodeResult::Succeeded;
}
