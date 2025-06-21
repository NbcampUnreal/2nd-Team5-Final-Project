#include "BTDecorator_CheckGamePlayTag.h"
#include "AIController.h"
#include "AI/RealAI/MonsterAICharacter.h"

UBTDecorator_CheckGamePlayTag::UBTDecorator_CheckGamePlayTag()
{
	NodeName = "Check Tag";
}

bool UBTDecorator_CheckGamePlayTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return false;

	AMonsterAICharacter* AIPawn = Cast<AMonsterAICharacter>(AICon->GetPawn());
	if (!AIPawn) return false;

	switch (TagType)
	{
	case ETagType::TT_State:
		return AIPawn->IsInPrimaryState(TagToCheck);
	case ETagType::TT_Battle:
		return AIPawn->HasBattleState(TagToCheck);
	case ETagType::TT_Strategy:
		return AIPawn->HasStrategyState(TagToCheck);
	case ETagType::TT_MonsterMode:
		return AIPawn->HasMonsterModeState(TagToCheck);
	}

	return false;
}