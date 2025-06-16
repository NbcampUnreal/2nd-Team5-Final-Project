// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objective/SLObjectiveManagerBase.h"
#include "SLBattleObjectiveManager.generated.h"

class ASLBattlePlayerState;

UCLASS()
class STILLLOADING_API ASLBattleObjectiveManager : public ASLObjectiveManagerBase
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void ActivatePlayerAllUI();

	UFUNCTION(BlueprintCallable)
	void ActivatePlayerHpUI();

	UFUNCTION(BlueprintCallable)
	void ActivatePlayerSpecialUI();

	UFUNCTION(BlueprintCallable)
	void ActivateBossHpUI();

	UFUNCTION(BlueprintCallable)
	void ActivateHitEffectUI();

	UFUNCTION(BlueprintCallable)
	void DeactivateBattleUI();

	void GetPlayerState();

protected:
	UPROPERTY()
	TObjectPtr<ASLBattlePlayerState> PlayerState = nullptr;
};
