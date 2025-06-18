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

USLBTTask_Attack::USLBTTask_Attack(): AttackType()
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

	ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(Target);
	if (!PlayerCharacter) return EBTNodeResult::Failed;

	if (AICharacter->HasBattleState(TAG_AI_Hit)
		|| AICharacter->HasBattleState(TAG_AI_IsAttacking)) return EBTNodeResult::Failed;

	if (AttackType == EAttackAnimType::AAT_AINormal)
	{
		if (FMath::RandRange(0, 1) == 0)
		{
			AnimComp->PlayAIAttackMontage("Attack1");
		}
		else
		{
			AnimComp->PlayAIAttackMontage("Attack2");
		}
	}
	else if (AttackType == EAttackAnimType::AAT_AISpecial)
	{
		AnimComp->PlayAIAttackMontage("Attack3");
	}
	
	AICharacter->SetBattleState(TAG_AI_IsAttacking);
	
	return EBTNodeResult::Succeeded;
}
