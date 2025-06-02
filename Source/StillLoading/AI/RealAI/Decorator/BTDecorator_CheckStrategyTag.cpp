#include "BTDecorator_CheckStrategyTag.h"

#include "AIController.h"
#include "AI/RealAI/MonsterAICharacter.h"

UBTDecorator_CheckStrategyTag::UBTDecorator_CheckStrategyTag()
{
	NodeName = "Check Strategy Tag";
}

bool UBTDecorator_CheckStrategyTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return false;

	AMonsterAICharacter* AIPawn = Cast<AMonsterAICharacter>(AICon->GetPawn());
	if (!AIPawn) return false;
	
	return AIPawn->HasStrategyState(TagToCheck);
}
