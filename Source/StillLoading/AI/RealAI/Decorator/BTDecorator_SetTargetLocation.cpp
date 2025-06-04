#include "BTDecorator_SetTargetLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_SetTargetLocation::UBTDecorator_SetTargetLocation()
{
	NodeName = "Set Target Location From Controller";
}

bool UBTDecorator_SetTargetLocation::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return false;

	FVector TargetLocation;
	if (Controller->GetBlackboardComponent()->GetValueAsVector("LeaderOrderLocation") != FVector::ZeroVector)
	{
		TargetLocation = Controller->GetBlackboardComponent()->GetValueAsVector("LeaderOrderLocation");
	}
	else
	{
		TargetLocation = Controller->GetPawn()->GetActorLocation();
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocationKey.SelectedKeyName, TargetLocation);
	return true;
}