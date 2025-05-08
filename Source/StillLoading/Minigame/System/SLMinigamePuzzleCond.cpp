// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigamePuzzleCond.h"

// Sets default values
ASLMinigamePuzzleCond::ASLMinigamePuzzleCond()
{

}

void ASLMinigamePuzzleCond::AddNumber(int32 Number)
{
	CurrentPermutation.Add(Number);
	if (CurrentPermutation.Num() == AnswerPermutation.Num())
	{
		CheckCondition();
	}
}

// Called when the game starts or when spawned
void ASLMinigamePuzzleCond::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLMinigamePuzzleCond::InitCondition()
{
	CurrentPermutation.Empty();
}

void ASLMinigamePuzzleCond::CheckCondition()
{
	for (int i = 0; i < CurrentPermutation.Num(); i++)
	{
		if (CurrentPermutation[i] != AnswerPermutation[i])
		{
			DeactivateStatue();
			return;
		}
	}
	bIsClear = true;
	SendCondition();
}

void ASLMinigamePuzzleCond::SendCondition()
{
}

void ASLMinigamePuzzleCond::DeactivateStatue()
{

}

