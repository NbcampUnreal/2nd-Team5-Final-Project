#include "BTDecorator_CheckGamePlayTag.h"
#include "AIController.h"
#include "AI/Testing/MonsterAICharacter.h"

UBTDecorator_CheckGamePlayTag::UBTDecorator_CheckGamePlayTag()
{
	NodeName = "Check Gameplay Tag";
}

bool UBTDecorator_CheckGamePlayTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return false;

	AMonsterAICharacter* AIPawn = Cast<AMonsterAICharacter>(AICon->GetPawn());
	if (!AIPawn) return false;
	
	return AIPawn->IsInPrimaryState(TagToCheck);
}