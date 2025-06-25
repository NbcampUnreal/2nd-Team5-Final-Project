#include "SLBTTask_Attack.h"

#include "AIController.h"
#include "AI/RealAI/Blackboardkeys.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"

USLBTTask_Attack::USLBTTask_Attack()
{
	NodeName = "Attack Player";
}

EBTNodeResult::Type USLBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	AMonsterAICharacter* AICharacter = Cast<AMonsterAICharacter>(AIPawn);
	if (!AICharacter) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(BlackboardKeys::TargetActor));
	if (!Target) return EBTNodeResult::Failed;

	UBattleComponent* BattleComp = AIPawn->FindComponentByClass<UBattleComponent>();
	if (!BattleComp) return EBTNodeResult::Failed;

	UAnimationMontageComponent* AnimComp = AIPawn->FindComponentByClass<UAnimationMontageComponent>();
	if (!AnimComp) return EBTNodeResult::Failed;

	if (AICharacter->HasBattleState(TAG_AI_Hit)
		|| AICharacter->HasBattleState(TAG_AI_IsAttacking))
		return EBTNodeResult::Failed;

	int8 RandNum = FMath::RandRange(0, 2);
	if (RandNum == 0)
	{
		AnimComp->PlayAIAttackMontage("Attack1");
	}
	else if (RandNum == 1)
	{
		AnimComp->PlayAIAttackMontage("Attack2");
	}
	else
	{
		AnimComp->PlayAIAttackMontage("Attack3");
	}
	AICharacter->SetStrategyState(TAG_AI_IsPlayingMontage);
	AICharacter->SetBattleState(TAG_AI_IsAttacking);

	return EBTNodeResult::Succeeded;
}
