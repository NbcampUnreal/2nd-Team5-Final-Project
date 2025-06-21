#include "BTService_CheckArrival.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_CheckArrival::UBTService_CheckArrival()
{
	NodeName = TEXT("Check Swarm Arrival");
	
	Interval = 0.2f;
	RandomDeviation = 0.05f;
}

void UBTService_CheckArrival::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ControlledPawn || !BlackboardComp) return;

	FVector TargetLocation = FVector::ZeroVector;
	bool bHasValidTarget = false;

	if (AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName)))
	{
		TargetLocation = TargetActor->GetActorLocation();
		bHasValidTarget = true;
	}
	else if (BlackboardComp->IsVectorValueSet(PatrolTargetLocationKey.SelectedKeyName))
	{
		TargetLocation = BlackboardComp->GetValueAsVector(PatrolTargetLocationKey.SelectedKeyName);
		bHasValidTarget = true;
	}

	if (bHasValidTarget)
	{
		const float DistanceToGoal = FVector::Dist(ControlledPawn->GetActorLocation(), TargetLocation);
		BlackboardComp->SetValueAsBool(IsAtLocationKey.SelectedKeyName, DistanceToGoal < AcceptanceRadius);
	}
	else
	{
		BlackboardComp->SetValueAsBool(IsAtLocationKey.SelectedKeyName, false);
	}
}