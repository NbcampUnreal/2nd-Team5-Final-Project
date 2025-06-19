// Fill out your copyright notice in the Description page of Project Settings.


#include "Objective/ObjectiveManager/SLBossBattleObjectiveManager.h"
#include "Character/SLAIBaseCharacter.h"
#include "UI/HUD/SLInGameHUD.h"
#include "Objective/SLObjectiveHandlerBase.h"

void ASLBossBattleObjectiveManager::ActivateBattleUI(USLObjectiveHandlerBase* Component)
{
	Super::ActivateBattleUI(Component);

	if (IsValid(HUD) && HUD->GetIsActivated(ESLInGameActivateType::EIGA_BossHpBar))
	{
		ActivateBossHpUI();
	}
}

void ASLBossBattleObjectiveManager::DeactivateBattleUI(USLObjectiveHandlerBase* Component)
{
	Super::DeactivateBattleUI(Component);

	if (IsValid(HUD))
	{
		HUD->SetBossStateVisibility(false);
	}
}

void ASLBossBattleObjectiveManager::CheckBossDied(float MaxHp, float CurrentHp)
{
	if (CurrentHp <= 0)
	{
		PostBossDied();
	}
}

void ASLBossBattleObjectiveManager::ActivateBossHpUI()
{
	if (IsValid(TargetBoss))
	{
		HUD->ApplyBossHp(TargetBoss->GetBossHpChangedDelegate());

		if (!TargetBoss->GetBossHpChangedDelegate().OnBossHpChanged.IsAlreadyBound(this, &ThisClass::CheckBossDied))
		{
			TargetBoss->GetBossHpChangedDelegate().OnBossHpChanged.AddDynamic(this, &ThisClass::CheckBossDied);
		}
	}
}
