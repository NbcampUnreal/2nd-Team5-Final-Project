#include "BTDecorator_CheckDistanceToTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckDistanceToTarget::UBTDecorator_CheckDistanceToTarget()
{
	NodeName = TEXT("Check Distance To Target");
}

bool UBTDecorator_CheckDistanceToTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return false;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return false;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return false;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return false;

	float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());
	return Distance <= MaxDistance;
}