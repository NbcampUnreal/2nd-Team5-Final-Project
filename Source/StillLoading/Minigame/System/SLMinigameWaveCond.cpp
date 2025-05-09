// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameWaveCond.h"

void ASLMinigameWaveCond::AddCount(int32 count)
{
	CurrentKillCount += count;
	if (CurrentKillCount >= MaxKillCount)
	{
		if (CurrentWave < MaxWave)
		{
			StartNextWave();
		}
		else
		{
			SendCondition(ESLMinigameResult::Success);
		}
	}
}

void ASLMinigameWaveCond::InitCondition()
{
}

void ASLMinigameWaveCond::SendCondition(ESLMinigameResult result)
{
	Super::SendCondition(result);
}

void ASLMinigameWaveCond::StartNextWave()
{
	CurrentWave++;
	//Spawn Monster
}
