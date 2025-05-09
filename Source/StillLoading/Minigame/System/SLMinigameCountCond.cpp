// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameCountCond.h"

// Sets default values
ASLMinigameCountCond::ASLMinigameCountCond()
{

}

void ASLMinigameCountCond::AddCount(int32 InAddCount)
{
	Count += InAddCount;
	if (Count >= MaxCount)
	{
		SendConditionToMinigameSubsystem(ESLMinigameResult::EMR_Success);
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

void ASLMinigameCountCond::SendConditionToMinigameSubsystem(ESLMinigameResult InResult)
{
	Super::SendConditionToMinigameSubsystem(InResult);
}

