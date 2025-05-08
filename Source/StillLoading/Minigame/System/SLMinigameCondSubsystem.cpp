// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameCondSubsystem.h"

EResult USLMinigameCondSubsystem::GetMinigameResult(EMinigame minigame)
{
	return MinigameResult[minigame];
}

void USLMinigameCondSubsystem::SetMinigameResult(EMinigame minigame, EResult result)
{
	MinigameResult[minigame] = result;
}
