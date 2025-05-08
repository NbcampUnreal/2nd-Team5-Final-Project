// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameCountCond.h"

// Sets default values
ASLMinigameCountCond::ASLMinigameCountCond()
{

}

void ASLMinigameCountCond::AddCount(int32 addCount)
{
	Count += addCount;
	if (Count >= MaxCount)
	{
		SendCondition(ESLMinigameResult::Success);
	}
	
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

void ASLMinigameCountCond::SendCondition(ESLMinigameResult result)
{
	Super::SendCondition(result);
}

