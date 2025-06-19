// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objective/SLObjectiveManagerBase.h"
#include "SLBattleObjectiveManager.generated.h"

UCLASS()
class STILLLOADING_API ASLBattleObjectiveManager : public ASLObjectiveManagerBase
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	virtual void ActivateBattleUI(USLObjectiveHandlerBase* Component);

	UFUNCTION(BlueprintCallable)
	virtual void DeactivateBattleUI(USLObjectiveHandlerBase* Component);

	void ActivatePlayerSpecialUI();
	void ActivateHitEffectUI();

	virtual void BeginPlay() override;
};
