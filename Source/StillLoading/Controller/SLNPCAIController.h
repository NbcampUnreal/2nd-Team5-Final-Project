// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseAIController.h"
#include "SLNPCAIController.generated.h"

UCLASS()
class STILLLOADING_API ASLNPCAIController : public ASLBaseAIController
{
	GENERATED_BODY()

public:
	ASLNPCAIController();

	//~IGenericTeamAgentInterface inteface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//End of ~IGenericTeamAgentInterface inteface
	
protected:
	virtual void BeginPlay() override;
};
