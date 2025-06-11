// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SLBTService_CheckPlayerJump.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLBTService_CheckPlayerJump : public UBTService
{
    GENERATED_BODY()
public:
    USLBTService_CheckPlayerJump();
    
protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // 플레이어 점프 상태를 저장할 블랙보드 키
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blackboard")
    FBlackboardKeySelector PlayerIsJumpingKey;

    // 플레이어가 새로 점프를 시작했는지를 저장할 블랙보드 키
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blackboard")
    FBlackboardKeySelector PlayerStartedJumpingKey;

private:
    bool bPreviousPlayerJumpState;
};
