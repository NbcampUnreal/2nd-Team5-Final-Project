// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SLBTTask_PerformJumpAttack.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLBTTask_PerformJumpAttack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    USLBTTask_PerformJumpAttack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    // 점프 공격 완료까지 대기할지 여부
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Attack")
    bool bWaitForJumpAttackComplete;

    // 점프 공격 타임아웃 (초)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump Attack")
    float JumpAttackTimeout;

private:
    float TaskStartTime;
};
