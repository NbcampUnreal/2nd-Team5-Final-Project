// Fill out your copyright notice in the Description page of Project Settings.


#include "Objective/ObjectiveManager/SLBattleObjectiveManager.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
#include "UI/HUD/SLInGameHUD.h"
#include "Character/SLAIBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

void ASLBattleObjectiveManager::ActivatePlayerAllUI()
{
	ActivatePlayerHpUI();
	ActivatePlayerSpecialUI();
}

void ASLBattleObjectiveManager::ActivatePlayerHpUI()
{
	GetPlayerState();

	if (IsValid(PlayerState) && IsValid(HUD))
	{
		HUD->ApplyPlayerHp(PlayerState->GetHPDelegate());
	}
}

void ASLBattleObjectiveManager::ActivatePlayerSpecialUI()
{
	GetPlayerState();

	if (IsValid(PlayerState) && IsValid(HUD))
	{
		HUD->ApplyPlayerSpecial(PlayerState->GetGageDelegate());
	}
}

void ASLBattleObjectiveManager::ActivateHitEffectUI()
{
	GetPlayerState();

	if (IsValid(PlayerState) && IsValid(HUD))
	{
		HUD->ApplyHitEffect(PlayerState->GetHPDelegate());
	}
}

void ASLBattleObjectiveManager::DeactivateBattleUI()
{
	if (IsValid(HUD))
	{
		HUD->SetBossStateVisibility(false);
		HUD->SetPlayerStateVisibility(false, false);
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

		if (IsValid(BossCharacter) && IsValid(HUD))
		{
			// TODO : Compare BossCharacter Type
			// if (BossCharacter->GetIsBoss())
			HUD->ApplyBossHp(BossCharacter->GetBossHpChangedDelegate());
			break;
		}
	}
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