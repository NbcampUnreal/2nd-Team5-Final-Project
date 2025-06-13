// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigamePuzzleCond.h"

// Sets default values
ASLMinigamePuzzleCond::ASLMinigamePuzzleCond()
{
	CurrentStates = {};
	AnswerStates = {};
	Statues = {};
}

void ASLMinigamePuzzleCond::UpdateStatueState(int8 InStatueIndex, int8 SubmittedValue)
{
	

	switch (PuzzleType)
	{
	case ESLPuzzleType::RotatePuzzle:
		if (CurrentStates.IsValidIndex(InStatueIndex))
		{
			CurrentStates[InStatueIndex] = SubmittedValue;
			TryCount++;
		}
		SubmittedAnswer();
		break;
	case ESLPuzzleType::LightPuzzle:
		if (CurrentStates.IsValidIndex(InStatueIndex))
		{
			CurrentStates[TryCount] = SubmittedValue;
			TryCount++;
			for (int32 i = 0; i < CurrentStates.Num(); i++)
			{
				if (CurrentStates[i] < 1)
				{
					return;
				}
			}
			
			SubmittedAnswer();
		}
		
		break;
	default:
		break;
	}
	
}

void ASLMinigamePuzzleCond::SubmittedAnswer()
{
	for (int i = 0; i < CurrentStates.Num(); i++)
	{
		if (CurrentStates[i] != AnswerStates[i])
		{
			GameSucceedFlag = false;
			break;
		}
		
		GameSucceedFlag = true;
	}

	if (GameSucceedFlag)
	{
		SendCondition(ESLMinigameResult::EMR_Success);
	}
	else if (TryCount > 5) // 임시
	{
		DeactivateAllStatue();
	}
	/*else
	{
		DeactivateAllStatue();
	}*/
	
}

void ASLMinigamePuzzleCond::RegisterStatue(ASLInteractableObjectStatue* InStatue)
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
		CurrentStates[i] = 0;
	}
	ResetCondition();

}

void ASLMinigamePuzzleCond::ResetCondition()
{
	ObjectResetRequested.Broadcast();
	TryCount = 0;
}

