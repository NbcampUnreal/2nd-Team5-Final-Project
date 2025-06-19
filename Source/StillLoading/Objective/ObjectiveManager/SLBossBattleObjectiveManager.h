// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objective/ObjectiveManager/SLBattleObjectiveManager.h"
#include "SLBossBattleObjectiveManager.generated.h"

class ASLAIBaseCharacter;

UCLASS()
class STILLLOADING_API ASLBossBattleObjectiveManager : public ASLBattleObjectiveManager
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess = "true"))
	void PostBossDied();

	UFUNCTION()
	void CheckBossDied(float MaxHp, float CurrentHp);

	virtual void ActivateBattleUI(USLObjectiveHandlerBase* Component) override;
	virtual void DeactivateBattleUI(USLObjectiveHandlerBase* Component) override;

private:
	void ActivateBossHpUI();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	TObjectPtr<ASLAIBaseCharacter> TargetBoss = nullptr;
};
