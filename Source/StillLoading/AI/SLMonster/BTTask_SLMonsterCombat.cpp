// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SLMonster/BTTask_SLMonsterCombat.h"
#include "AIController.h"
#include "AI/SLMonster/SLMonster.h"

UBTTask_SLMonsterCombat::UBTTask_SLMonsterCombat()
{
	NodeName = TEXT("SLMonster Attack");
}

EBTNodeResult::Type UBTTask_SLMonsterCombat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* AIPawn = AIController->GetPawn();
	ASLMonster* Monster = Cast<ASLMonster>(AIPawn);
	if (!Monster->IsTargetInSight())
	{
		return EBTNodeResult::Failed;
	}
	Monster->Attack();
	return EBTNodeResult::Succeeded;
}