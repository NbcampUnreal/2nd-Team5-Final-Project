#include "BTTask_GetNextPatrolPoint.h"

#include "AIController.h"
#include "AI/RealAI/Boid/SwarmManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

UBTTask_GetNextPatrolPoint::UBTTask_GetNextPatrolPoint()
{
	NodeName = TEXT("Get Next Patrol Point");
}

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BlackboardComp) return EBTNodeResult::Failed;

	ASwarmManager* SwarmManager = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass()));
	if (!SwarmManager || SwarmManager->SwarmPatrolPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetNextPatrolPoint: SwarmManager not found or has no patrol points."));
		return EBTNodeResult::Failed;
	}

	const int32 CurrentIndex = BlackboardComp->GetValueAsInt(PatrolIndexKey.SelectedKeyName);

	const ATargetPoint* TargetPoint = SwarmManager->SwarmPatrolPoints[CurrentIndex];
	if (!TargetPoint) return EBTNodeResult::Failed;
    
	BlackboardComp->SetValueAsVector(PatrolLocationKey.SelectedKeyName, TargetPoint->GetActorLocation());

	int32 NextIndex = (CurrentIndex + 1) % SwarmManager->SwarmPatrolPoints.Num();
	BlackboardComp->SetValueAsInt(PatrolIndexKey.SelectedKeyName, NextIndex);

	return EBTNodeResult::Succeeded;
}