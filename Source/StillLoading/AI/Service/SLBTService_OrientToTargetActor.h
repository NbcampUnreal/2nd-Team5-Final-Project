// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SLBTService_OrientToTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLBTService_OrientToTargetActor : public UBTService
{
    GENERATED_BODY()
public:
    USLBTService_OrientToTargetActor();

protected:
    //~ Begin UBTNode Interface
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
    virtual FString GetStaticDescription() const override;
    //~ End UBTNode Interface

    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    UPROPERTY(EditAnywhere, Category = "Target")
    FBlackboardKeySelector InTargetActorKey;

    UPROPERTY(EditAnywhere, Category = "Target")
    float RotationInterpSpeed;
    
};
