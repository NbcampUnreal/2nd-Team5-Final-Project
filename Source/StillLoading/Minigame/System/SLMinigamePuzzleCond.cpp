// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigamePuzzleCond.h"

// Sets default values
ASLMinigamePuzzleCond::ASLMinigamePuzzleCond()
{
	CurrentPermutation = {};
	AnswerPermutation = {};
}

void ASLMinigamePuzzleCond::AddNumber(int32 Number)
{
	CurrentPermutation.Add(Number);
	if (CurrentPermutation.Num() == AnswerPermutation.Num())
	{
		for (int i = 0; i < CurrentPermutation.Num(); i++)
		{
			if (CurrentPermutation[i] != AnswerPermutation[i])
			{
				DeactivateStatue();
				return;
			}
		}
		SendCondition(ESLMinigameResult::Success);
	}
}

// Called when the game starts or when spawned
void ASLMinigamePuzzleCond::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLMinigamePuzzleCond::InitCondition()
{
	Super::InitCondition();
	CurrentPermutation.Empty();
}

void ASLMinigamePuzzleCond::SendCondition(ESLMinigameResult result)
{
	Super::SendCondition(result);
}

void ASLMinigamePuzzleCond::DeactivateStatue()
{

}

