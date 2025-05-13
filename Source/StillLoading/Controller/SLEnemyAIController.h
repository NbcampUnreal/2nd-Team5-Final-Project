// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseAIController.h"
#include "SLEnemyAIController.generated.h"

UCLASS()
class STILLLOADING_API ASLEnemyAIController : public ASLBaseAIController
{
	GENERATED_BODY()

public:
	ASLEnemyAIController();

	//~IGenericTeamAgentInterface inteface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//End of ~IGenericTeamAgentInterface inteface
	
protected:
	virtual void BeginPlay() override;

};
