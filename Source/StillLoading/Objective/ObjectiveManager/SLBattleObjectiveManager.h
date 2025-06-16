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
	void ActivateBattleUI(USLObjectiveHandlerBase* Component);

	UFUNCTION(BlueprintCallable)
	void DeactivateBattleUI(USLObjectiveHandlerBase* Component);

	UFUNCTION(BlueprintCallable)
	void ActivatePlayerHpUI();

	UFUNCTION(BlueprintCallable)
	void ActivatePlayerSpecialUI();

	UFUNCTION(BlueprintCallable)
	void ActivateBossHpUI();

	UFUNCTION(BlueprintCallable)
	void ActivateHitEffectUI();

	virtual void BeginPlay() override;
	void GetPlayerState();

protected:
	UPROPERTY()
	TObjectPtr<ASLBattlePlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective", meta = (AllowPrivateAccess = "true"))
	bool bIsBossBattle = false;
};
