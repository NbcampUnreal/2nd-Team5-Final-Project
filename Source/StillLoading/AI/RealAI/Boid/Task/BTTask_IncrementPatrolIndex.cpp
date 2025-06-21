#include "BTTask_IncrementPatrolIndex.h"

#include "AI/RealAI/Boid/SwarmManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_IncrementPatrolIndex::UBTTask_IncrementPatrolIndex()
{
	NodeName = "Increment Patrol Index";
}

EBTNodeResult::Type UBTTask_IncrementPatrolIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const ASwarmManager* SwarmManager = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass()));

	if (!BlackboardComp || !SwarmManager || SwarmManager->SwarmPatrolPoints.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	const int32 CurrentIndex = BlackboardComp->GetValueAsInt(PatrolIndexKey.SelectedKeyName);

	const int32 NumPatrolPoints = SwarmManager->SwarmPatrolPoints.Num();
	const int32 NextIndex = (CurrentIndex + 1) % NumPatrolPoints;

	BlackboardComp->SetValueAsInt(PatrolIndexKey.SelectedKeyName, NextIndex);
	
	return EBTNodeResult::Succeeded;
}