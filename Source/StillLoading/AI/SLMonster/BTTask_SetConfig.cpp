// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SLMonster/BTTask_SetConfig.h"
#include "AIController.h"
#include "Controller/SLEnemyAIController.h"

UBTTask_SetConfig::UBTTask_SetConfig()
{
	NodeName = TEXT("Set AI Config");
}

EBTNodeResult::Type UBTTask_SetConfig::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ASLEnemyAIController* MonsterController = Cast<ASLEnemyAIController>(AIController);
	if (MonsterController)
	{
		MonsterController->bHasFixedTarget = bSetHasFixedTarget;
		MonsterController->SetPeripheralVisionAngle(VisionAngle);

		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject("TargetActor"));
		MonsterController->SetFocus(TargetActor);
		ASLMonster* Monster = Cast<ASLMonster>(MonsterController->GetPawn());
		Monster->SetRVOAvoidanceEnabled(bEnableRVO);
		Monster->SetIsInCombat(IsInCombat);
		Blackboard->SetValueAsBool("IsInCombat", IsInCombat);
		return EBTNodeResult::Succeeded;
	}

	
	return EBTNodeResult::Failed;
}
