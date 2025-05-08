// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameCondSubsystem.h"

ESLMinigameResult USLMinigameCondSubsystem::GetMinigameResult(ESLMinigameStage minigame)
{
	return MinigameResult[minigame];
}

void USLMinigameCondSubsystem::SetMinigameResult(ESLMinigameStage minigame, ESLMinigameResult result)
{
	MinigameResult[minigame] = result;
}
