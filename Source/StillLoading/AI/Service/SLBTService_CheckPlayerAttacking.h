// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SLBTService_CheckPlayerAttacking.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLBTService_CheckPlayerAttacking : public UBTService
{
    GENERATED_BODY()

public:
    USLBTService_CheckPlayerAttacking();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blackboard")
    FBlackboardKeySelector IsPlayerAttackingKey;
};
