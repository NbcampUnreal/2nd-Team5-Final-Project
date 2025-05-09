// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameCondSubsystem.h"

ESLMinigameResult USLMinigameCondSubsystem::GetMinigameResult(ESLMinigameStage InMinigame)
{
	if (MinigameResult.Contains(InMinigame))
	{
		return MinigameResult[InMinigame];
	}
	else
	{
		if (!MinigameResult.Contains(ESLMinigameStage::EMS_None))
		{
			MinigameResult.Add(ESLMinigameStage::EMS_None, ESLMinigameResult::EMR_None);
		}
		return MinigameResult[ESLMinigameStage::EMS_None];
	}
}

void USLMinigameCondSubsystem::SetMinigameResult(ESLMinigameStage InMinigame, ESLMinigameResult InResult)
{
	if (MinigameResult.Contains(InMinigame))
	{
		MinigameResult[InMinigame] = InResult;
	}
	else
	{
		MinigameResult.Add(InMinigame, InResult);
	}
}
