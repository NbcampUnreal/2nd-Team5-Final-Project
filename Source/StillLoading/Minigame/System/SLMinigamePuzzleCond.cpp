// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigamePuzzleCond.h"
#include "Minigame/Object/SLReactiveObjectStatue.h"

// Sets default values
ASLMinigamePuzzleCond::ASLMinigamePuzzleCond()
{
	CurrentStates = {};
	AnswerStates = {};
	Statues = {};
}

void ASLMinigamePuzzleCond::UpdateStatueState(int8 InStatueIndex, int8 SubmittedValue)
{
	if (CurrentStates.IsValidIndex(InStatueIndex))
	{
		CurrentStates[InStatueIndex] = SubmittedValue;
	}
	if (!CurrentStates.Contains(-1))
	{
		SubmittedAnswer();
	}
}

void ASLMinigamePuzzleCond::SubmittedAnswer()
{
	for (int i = 0; i < CurrentStates.Num(); i++)
	{
		if (CurrentStates[i] != AnswerStates[i])
		{
			DeactivateAllStatue();
			return;
		}
	}
	SendCondition(ESLMinigameResult::EMR_Success);
}

void ASLMinigamePuzzleCond::RegisterStatue(ASLReactiveObjectStatue* InStatue)
{
	Statues.Add(InStatue);
}

// Called when the game starts or when spawned
void ASLMinigamePuzzleCond::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLMinigamePuzzleCond::InitCondition()
{
	Super::InitCondition();
	CurrentStates.Empty();
}

void ASLMinigamePuzzleCond::SendCondition(ESLMinigameResult InResult)
{
	Super::SendConditionToMinigameSubsystem(InResult);
}

void ASLMinigamePuzzleCond::DeactivateAllStatue()
{
	for (int i = 0; i < CurrentStates.Num(); i++)
	{
		CurrentStates[i] = -1;
		Statues[i];
	}
}

