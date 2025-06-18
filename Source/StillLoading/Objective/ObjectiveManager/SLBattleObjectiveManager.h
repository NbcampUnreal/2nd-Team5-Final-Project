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
	virtual void ActivateBattleUI(USLObjectiveHandlerBase* Component);

	UFUNCTION(BlueprintCallable)
	virtual void DeactivateBattleUI(USLObjectiveHandlerBase* Component);

	UFUNCTION(BlueprintCallable)
	void PostPlayerDiedTest();

	UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess = "true"))
	void PostPlayerDied();

	UFUNCTION()
	void CheckPlayerDied(float MaxHp, float CurrentHp);

	void ActivatePlayerHpUI();
	void ActivatePlayerSpecialUI();
	void ActivateHitEffectUI();

	virtual void BeginPlay() override;
	void GetPlayerState();

protected:
	UPROPERTY()
	TObjectPtr<ASLBattlePlayerState> PlayerState = nullptr;
};
