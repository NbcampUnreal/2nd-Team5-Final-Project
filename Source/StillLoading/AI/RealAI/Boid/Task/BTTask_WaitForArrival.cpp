#include "BTTask_WaitForArrival.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WaitForArrival::UBTTask_WaitForArrival()
{
	NodeName = "Wait For Arrival";
	
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WaitForArrival::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTTask_WaitForArrival::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* AIController = OwnerComp.GetAIOwner();
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!AIController || !BlackboardComp || !ControlledPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!BlackboardComp->IsVectorValueSet(TargetLocationKey.SelectedKeyName))
	{
		UE_LOG(LogTemp, Warning, TEXT("WaitForArrival Failed: Blackboard key '%s' is not set."), *TargetLocationKey.SelectedKeyName.ToString());
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	const FVector TargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	const float DistanceToGoal = FVector::Dist(ControlledPawn->GetActorLocation(), TargetLocation);

	if (DistanceToGoal < AcceptanceRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}