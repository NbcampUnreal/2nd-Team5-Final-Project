#include "BTService_CheckPatrolCondition.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_CheckPatrolCondition::UBTService_CheckPatrolCondition()
{
	NodeName = "Check Patrol Condition";
	Interval = 0.2f; // 0.2초마다 검사
}

void UBTService_CheckPatrolCondition::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	const bool bIsAtLocation = BlackboardComp->GetValueAsBool(IsAtLocationKey.SelectedKeyName);
	const bool bPatrolLocationIsNotSet = !BlackboardComp->IsVectorValueSet(PatrolTargetLocationKey.SelectedKeyName);
	const bool bShouldGetNewPoint = bIsAtLocation || bPatrolLocationIsNotSet;

	BlackboardComp->SetValueAsBool(ResultShouldGetKey.SelectedKeyName, bShouldGetNewPoint);
}