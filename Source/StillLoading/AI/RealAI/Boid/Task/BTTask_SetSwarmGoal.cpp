#include "BTTask_SetSwarmGoal.h"

#include "AI/RealAI/Blackboardkeys.h"
#include "AI/RealAI/Boid/SwarmManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_SetSwarmGoal::UBTTask_SetSwarmGoal()
{
	NodeName = TEXT("Set Swarm Goal");
}

EBTNodeResult::Type UBTTask_SetSwarmGoal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	ASwarmManager* SwarmManager = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass()));
	if (!SwarmManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_SetSwarmGoal: SwarmManager not found in world."));
		return EBTNodeResult::Failed;
	}

	FVector TargetLocation;
    
	if (UObject* TargetObject = BlackboardComp->GetValueAsObject(TargetLocationKey.SelectedKeyName))
	{
		if (AActor* TargetActor = Cast<AActor>(TargetObject))
		{
			TargetLocation = TargetActor->GetActorLocation();
		}
	}
	else
	{
		TargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	}

	SwarmManager->SwarmGoalLocation = TargetLocation;
	SwarmManager->bHasGoal = true;

	return EBTNodeResult::Succeeded;
}