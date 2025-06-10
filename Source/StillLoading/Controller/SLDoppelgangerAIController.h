// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseAIController.h"
#include "SLDoppelgangerAIController.generated.h"

UCLASS()
class STILLLOADING_API ASLDoppelgangerAIController : public ASLBaseAIController
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASLDoppelgangerAIController();

    //~IGenericTeamAgentInterface inteface
    virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
    //End of ~IGenericTeamAgentInterface inteface

protected:

    virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
