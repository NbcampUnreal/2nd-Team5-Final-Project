#include "SLBTTask_Attack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BattleComponent/BattleComponent.h"
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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!Target) return EBTNodeResult::Failed;

	UBattleComponent* BattleComp = AIPawn->FindComponentByClass<UBattleComponent>();
	if (!BattleComp) return EBTNodeResult::Failed;

	UAnimationMontageComponent* AnimComp = AIPawn->FindComponentByClass<UAnimationMontageComponent>();
	if (!AnimComp) return EBTNodeResult::Failed;
	
	BattleComp->DoAttackSweep(EAttackAnimType::AAT_AINormal);
	AnimComp->PlayAIAttackMontage();

	// 공격 쿨다운 설정
	BlackboardComp->SetValueAsBool("CanAttack", false);

	// 일정 시간 후 CanAttack을 true로 설정하는 타이머를 설정합니다.
	FTimerHandle TimerHandle;
	AIPawn->GetWorldTimerManager().SetTimer(TimerHandle, [BlackboardComp]()
	{
		BlackboardComp->SetValueAsBool("CanAttack", true);
	}, 2.0f, false);

	return EBTNodeResult::Succeeded;
}
