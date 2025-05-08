// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameWaveCond.h"

void ASLMinigameWaveCond::AddCount(int count)
{
	CurrentKillCount += count;
	if (CurrentKillCount >= MaxKillCount)
	{
		CheckCondition(EResult::Success);
	}
}

void ASLMinigameWaveCond::InitCondition()
{
}

void ASLMinigameWaveCond::CheckCondition(EResult result)
{
	switch (result)
	{
	case EResult::Success:
		if (CurrentWave < MaxWave)
		{
			StartNextWave();
			return;
		}
		break;
	case EResult::Fail:
		break;
	case EResult::Pass:
		break;
	default:
		break;
	}
	SendCondition(result);
}

void ASLMinigameWaveCond::SendCondition(EResult result)
{
	Super::SendCondition(result);
}

void ASLMinigameWaveCond::StartNextWave()
{
	CurrentWave++;
	//Spawn Monster
}
