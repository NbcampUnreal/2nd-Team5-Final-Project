#include "SLBTTask_PlayWaitAnim.h"

#include "AIController.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "AI/RealAI/Boid/SwarmManager.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"

USLBTTask_PlayWaitAnim::USLBTTask_PlayWaitAnim()
{
	NodeName = "Play Wait Anim";
}

EBTNodeResult::Type USLBTTask_PlayWaitAnim::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	AMonsterAICharacter* AICharacter = Cast<AMonsterAICharacter>(AIPawn);
	if (!AICharacter) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	UAnimationMontageComponent* AnimComp = AIPawn->FindComponentByClass<UAnimationMontageComponent>();
	if (!AnimComp) return EBTNodeResult::Failed;

	if (AICharacter->HasBattleState(TAG_AI_Hit)
		|| AICharacter->HasStrategyState(TAG_AI_IsPlayingMontage))
		return EBTNodeResult::Failed;

	const ASwarmManager* SwarmManager = AICharacter->GetMySwarmManager();
	if (!SwarmManager)
	{
		return EBTNodeResult::Failed;
	}

	for (const TObjectPtr<ASwarmAgent> SwarmAgent : SwarmManager->GetAgents())
	{
		SwarmAgent->PlayETCWaitAnim();
	}

	return EBTNodeResult::Succeeded;
}