// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetConfig.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API UBTTask_SetConfig : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_SetConfig();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	float VisionAngle = 180.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	bool bSetHasFixedTarget = true;

	UPROPERTY(EditAnywhere, Category = "Config")
	bool bEnableRVO = true;
	UPROPERTY(EditAnywhere, Category = "Config")
	bool IsInCombat = false;
};
