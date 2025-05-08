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
		CheckCondition(EResult::Success);
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

void ASLMinigamePuzzleCond::CheckCondition(EResult result)
{
	switch (result)
	{
	case EResult::Success:
		for (int i = 0; i < CurrentPermutation.Num(); i++)
		{
			if (CurrentPermutation[i] != AnswerPermutation[i])
			{
				DeactivateStatue();
				return;
			}
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

void ASLMinigamePuzzleCond::SendCondition(EResult result)
{
	Super::SendCondition(result);
}

void ASLMinigamePuzzleCond::DeactivateStatue()
{

}

