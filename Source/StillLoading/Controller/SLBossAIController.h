// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseAIController.h"
#include "SLBossAIController.generated.h"

UCLASS()
class STILLLOADING_API ASLBossAIController : public ASLBaseAIController
{
	GENERATED_BODY()

public:
	ASLBossAIController();

	//~IGenericTeamAgentInterface inteface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//End of ~IGenericTeamAgentInterface inteface

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
