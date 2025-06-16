// Fill out your copyright notice in the Description page of Project Settings.


#include "Objective/ObjectiveManager/SLBattleObjectiveManager.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
#include "UI/HUD/SLInGameHUD.h"
#include "Character/SLAIBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Objective/SLObjectiveHandlerBase.h"

void ASLBattleObjectiveManager::ActivateBattleUI(USLObjectiveHandlerBase* Component)
{
	if (IsValid(HUD))
	{
		if (HUD->GetIsActivated(ESLInGameActivateType::EIGA_PlayerHpBar))
		{
			ActivatePlayerHpUI();
		}
		else if (HUD->GetIsActivated(ESLInGameActivateType::EIGA_PlayerHitEffect))
		{
			ActivateHitEffectUI();
		}

		if (HUD->GetIsActivated(ESLInGameActivateType::EIGA_PlayerSpecialBar))
		{
			ActivatePlayerSpecialUI();
		}

		if (bIsBossBattle && HUD->GetIsActivated(ESLInGameActivateType::EIGA_BossHpBar))
		{
			ActivateBossHpUI();
		}
	}
}

void ASLBattleObjectiveManager::DeactivateBattleUI(USLObjectiveHandlerBase* Component)
{
	if (IsValid(HUD))
	{
		HUD->SetBossStateVisibility(false);
		HUD->SetPlayerStateVisibility(false, false);
	}
}

void ASLBattleObjectiveManager::ActivatePlayerHpUI()
{
	GetPlayerState();

	if (IsValid(PlayerState))
	{
		HUD->ApplyPlayerHp(PlayerState->GetHPDelegate());
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
	}
}

void ASLBattleObjectiveManager::ActivateBossHpUI()
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASLAIBaseCharacter::StaticClass(), AllActors);

	if (AllActors.IsEmpty())
	{
		return;
	}

	for (AActor* Actor : AllActors)
	{
		ASLAIBaseCharacter* BossCharacter = Cast<ASLAIBaseCharacter>(Actor);

		if (IsValid(BossCharacter))
		{
			if (BossCharacter->GetISBoss())
			{
				HUD->ApplyBossHp(BossCharacter->GetBossHpChangedDelegate());
				break;
			}
		}
	}
}

void ASLBattleObjectiveManager::BeginPlay()
{
	Super::BeginPlay();

	ObjectiveHandler->OnObjectiveInProgressedDelegate.AddDynamic(this, &ThisClass::ActivateBattleUI);
	ObjectiveHandler->OnObjectiveCompletedDelegate.AddDynamic(this, &ThisClass::DeactivateBattleUI);
}

void ASLBattleObjectiveManager::GetPlayerState()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (IsValid(PC))
	{
		APlayerState* PS = PC->GetPawn()->GetPlayerState();

		if (IsValid(PS))
		{
			PlayerState = Cast<ASLBattlePlayerState>(PS);
		}
	}
}