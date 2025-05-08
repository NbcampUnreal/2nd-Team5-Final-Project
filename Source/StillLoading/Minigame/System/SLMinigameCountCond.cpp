// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameCountCond.h"

// Sets default values
ASLMinigameCountCond::ASLMinigameCountCond()
{

}

void ASLMinigameCountCond::AddCount(int32 addCount)
{
	Count += addCount;
	CheckCondition();
}

// Called when the game starts or when spawned
void ASLMinigameCountCond::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLMinigameCountCond::InitCondition()
{
	Count = 0;
	bIsClear = false;
}

void ASLMinigameCountCond::CheckCondition()
{
	if (Count >= MaxCount)
	{
		SendCondition();
	}
}

void ASLMinigameCountCond::SendCondition()
{
}

