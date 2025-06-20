#include "BTService_CheckArrival.h"

#include "AIController.h"
#include "AI/RealAI/Blackboardkeys.h"
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
	bool bHasTarget = false;

	if (AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BlackboardKeys::TargetActor)))
	{
		TargetLocation = TargetActor->GetActorLocation();
		bHasTarget = true;
	}
	else
	{
		TargetLocation = BlackboardComp->GetValueAsVector(TEXT("PatrolTargetLocation"));
		if (!TargetLocation.IsZero())
		{
			bHasTarget = true;
		}
	}

	if (bHasTarget)
	{
		float DistanceToGoal = FVector::Dist(ControlledPawn->GetActorLocation(), TargetLocation);
		bool bIsAtLocation = (DistanceToGoal < AcceptanceRadius);
		BlackboardComp->SetValueAsBool(IsAtLocationKey.SelectedKeyName, bIsAtLocation);
	}
	else
	{
		BlackboardComp->SetValueAsBool(IsAtLocationKey.SelectedKeyName, false);
	}
}