// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameWaveCond.h"

void ASLMinigameWaveCond::AddCount(int32 InCount)
{
	CurrentKillCount += InCount;
	if (CurrentKillCount >= MaxKillCount)
	{
		if (CurrentWave < MaxWave)
		{
			StartNextWave();
		}
		else
		{
			SendCondition(ESLMinigameResult::EMR_Success);
		}
	}
}

void ASLMinigameWaveCond::InitCondition()
{
}

void ASLMinigameWaveCond::SendCondition(ESLMinigameResult InResult)
{
	Super::SendConditionToMinigameSubsystem(InResult);
}

void ASLMinigameWaveCond::StartNextWave()
{
	CurrentWave++;
	//Spawn Monster
}
