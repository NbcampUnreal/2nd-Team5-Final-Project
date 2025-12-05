// Fill out your copyright notice in the Description page of Project Settings.


#include "Objective/ObjectiveManager/SLBattleObjectiveManager.h"

#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/SLInGameHUD.h"
#include "Objective/SLObjectiveHandlerBase.h"

void ASLBattleObjectiveManager::ActivateBattleUI(USLObjectiveHandlerBase* Component)
{
	if (IsValid(HUD))
	{
		if (HUD->GetIsActivated(ESLInGameActivateType::EIGA_PlayerHpBar))
		{
			ActivatePlayerHpUI();
		}
		
		if (HUD->GetIsActivated(ESLInGameActivateType::EIGA_PlayerHitEffect))
		{
			ActivateHitEffectUI();
		}

		if (HUD->GetIsActivated(ESLInGameActivateType::EIGA_PlayerSpecialBar))
		{
			ActivatePlayerSpecialUI();
		}
	}
}

void ASLBattleObjectiveManager::DeactivateBattleUI(USLObjectiveHandlerBase* Component)
{
	if (IsValid(HUD))
	{
		HUD->SetPlayerStateVisibility(false, false);
	}
}

void ASLBattleObjectiveManager::ActivatePlayerSpecialUI()
{
	GetPlayerState();

	if (IsValid(PlayerState))
	{
		HUD->ApplyPlayerSpecial(PlayerState->GetGageDelegate());
	}
}

void ASLBattleObjectiveManager::ActivateHitEffectUI()
{
	GetPlayerState();

	if (IsValid(PlayerState))
	{
		HUD->ApplyHitEffect(PlayerState->GetHPDelegate());

		if (!PlayerState->GetHPDelegate().OnPlayerHpChanged.IsAlreadyBound(this, &ThisClass::CheckPlayerDied))
		{
			PlayerState->GetHPDelegate().OnPlayerHpChanged.AddDynamic(this, &ThisClass::CheckPlayerDied);
		}
	}
}

void ASLBattleObjectiveManager::BeginPlay()
{
	Super::BeginPlay();

	ObjectiveHandler->OnObjectiveInProgressedDelegate.AddDynamic(this, &ThisClass::ActivateBattleUI);
	ObjectiveHandler->OnObjectiveCompletedDelegate.AddDynamic(this, &ThisClass::DeactivateBattleUI);
	ObjectiveHandler->OnObjectiveFailedDelegate.AddDynamic(this, &ThisClass::DeactivateBattleUI);
}

void ASLBattleObjectiveManager::BindBattleManager()
{
	if (!IsValid(BattleManager))
	{
		AActor* BattleManagerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASLBattleManager::StaticClass());
		BattleManager = Cast<ASLBattleManager>(BattleManagerActor);
	}
}
