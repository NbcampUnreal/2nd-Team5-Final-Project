#include "BTDecorator_AttackCooldown.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_AttackCooldown::UBTDecorator_AttackCooldown()
{
	NodeName = "Check Attack Cooldown";
}

bool UBTDecorator_AttackCooldown::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return false;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return false;

	float LastAttackTime = Blackboard->GetValueAsFloat(GetSelectedBlackboardKey());
	float CurrentTime = Pawn->GetWorld()->GetTimeSeconds();

	return (CurrentTime - LastAttackTime) >= Cooldown;
}