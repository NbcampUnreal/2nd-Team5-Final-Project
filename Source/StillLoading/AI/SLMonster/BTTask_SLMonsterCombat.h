// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SLMonsterCombat.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API UBTTask_SLMonsterCombat : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_SLMonsterCombat();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
