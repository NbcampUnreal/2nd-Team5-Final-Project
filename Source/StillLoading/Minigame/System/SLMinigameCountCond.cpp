// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameCountCond.h"

// Sets default values
ASLMinigameCountCond::ASLMinigameCountCond()
{

}

void ASLMinigameCountCond::AddCount(int32 addCount)
{
	Count += addCount;
	CheckCondition(EResult::Success);
}

// Called when the game starts or when spawned
void ASLMinigameCountCond::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLMinigameCountCond::InitCondition()
{
	Super::InitCondition();
	Count = 0;
}

void ASLMinigameCountCond::CheckCondition(EResult result)
{
	switch (result)
	{
	case EResult::Success:
		if (Count < MaxCount)
		{
			return;
		}
		bIsClear = true;
		break;
	case EResult::Fail:
		bIsFail = true;
		break;
	case EResult::Pass:
		bIsPass = true;
		break;
	default:
		break;
	}
	SendCondition(result);
}

void ASLMinigameCountCond::SendCondition(EResult result)
{
	Super::SendCondition(result);
}

