// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameModeBase.h"

#include "GameState/SLGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Objective/SLObjectiveBase.h"

ASLGameModeBase::ASLGameModeBase()
{
	GameStateClass = ASLGameStateBase::StaticClass();
}

void ASLGameModeBase::AddInProgressObjective(USLObjectiveBase* Objective)
{
	SLGameState->GetInProgressedObjectives().AddUnique(Objective);
	OnInProgressObjectiveAdded.Broadcast(Objective);
	OnPrimaryInProgressObjectiveChanged.Broadcast(Objective);
}

void ASLGameModeBase::RemoveInProgressObjective(USLObjectiveBase* Objective)
{
	TArray<TObjectPtr<USLObjectiveBase>>& InProgressedObjectives = SLGameState->GetInProgressedObjectives();
	
	if (!InProgressedObjectives.Contains(Objective))
	{
		return;
	}

	InProgressedObjectives.Remove(Objective);
	OnInProgressObjectiveRemoved.Broadcast(Objective);

	if (InProgressedObjectives.IsEmpty() == false)
	{
		OnPrimaryInProgressObjectiveChanged.Broadcast(InProgressedObjectives.Top());
	}
}

USLObjectiveBase* ASLGameModeBase::GetPrimaryInProgressObjective()
{
	return SLGameState->GetInProgressedObjectives().Top();
}

void ASLGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SLGameState = Cast<ASLGameStateBase>(GameState);
	checkf(SLGameState, TEXT("GameState is not ASLGameStateBase"));

	//TODO : 세이브 시스템으로부터 저장된 InProgressedObjective 가져와서 Top에 BroadCast 하기
}

void ASLGameModeBase::LoadGame()
{
	
}

void ASLGameModeBase::LoadInProgressObjectives()
{
	for (USLObjectiveBase* InProgressObjective : SLGameState->GetInProgressedObjectives())
	{
		if (InProgressObjective)
		{
			AddInProgressObjective(InProgressObjective);
		}
	}
}
