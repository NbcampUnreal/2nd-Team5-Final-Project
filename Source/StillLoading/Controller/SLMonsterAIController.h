// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/SLBaseAIController.h"
#include "SLMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLMonsterAIController : public ASLBaseAIController
{
	GENERATED_BODY()

public:
    ASLMonsterAIController();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);
	
};
