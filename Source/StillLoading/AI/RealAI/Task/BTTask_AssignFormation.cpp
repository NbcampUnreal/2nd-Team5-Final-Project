#include "BTTask_AssignFormation.h"

#include "AI/RealAI/FormationComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AssignFormation::UBTTask_AssignFormation()
{
	NodeName = "Assign Formation";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_AssignFormation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* Leader = Cast<AActor>(BB->GetValueAsObject(FName("Leader")));
	if (!Leader) return EBTNodeResult::Failed;

	UFormationComponent* FormationComp = Leader->FindComponentByClass<UFormationComponent>();
	if (!FormationComp) return EBTNodeResult::Failed;

	FormationComp->AssignStoredFormation(FormationType, Spacing);

	return EBTNodeResult::Succeeded;
}