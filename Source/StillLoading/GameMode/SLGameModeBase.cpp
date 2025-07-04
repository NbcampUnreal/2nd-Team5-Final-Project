// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameModeBase.h"

#include "GameState/SLGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Objective/SLObjectiveBase.h"

ASLGameModeBase::ASLGameModeBase()
{
	GameStateClass = ASLGameStateBase::StaticClass();
}

void ASLGameModeBase::OnUpdatedObjectiveState(USLObjectiveBase* Objective, const ESLObjectiveState InState)
{
	TArray<TObjectPtr<USLObjectiveBase>>& ModifiedObjectives = SLGameState->GetModifedObjectives();
	ModifiedObjectives.AddUnique(Objective);

	switch (InState)
	{
	default:
		break;
	case ESLObjectiveState::InProgress:
		AddInProgressObjective(Objective);
		break;
	case ESLObjectiveState::Complete:
	case ESLObjectiveState::Fail:
		RemoveInProgressObjective(Objective);
		break;
	}
}

void ASLGameModeBase::AddInProgressObjective(USLObjectiveBase* Objective)
{
	TArray<TObjectPtr<USLObjectiveBase>>& InProgressedObjectives = SLGameState->GetInProgressedObjectives();
	
	if (!SLGameState)
	{
		return;
	}
	if (InProgressedObjectives.Contains(Objective))
	{
		return;
	}
	
	InProgressedObjectives.AddUnique(Objective);
	
	OnInProgressObjectiveAdded.Broadcast(Objective);
	OnPrimaryInProgressObjectiveChanged.Broadcast(Objective);
}

void ASLGameModeBase::RemoveInProgressObjective(USLObjectiveBase* Objective)
{
	TArray<TObjectPtr<USLObjectiveBase>>& InProgressedObjectives = SLGameState->GetInProgressedObjectives();
	if (!SLGameState)
	{
		return;
	}
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

void ASLGameModeBase::ResetModifiedObjectives()
{
	for (USLObjectiveBase* ModifiedObjective : SLGameState->GetInProgressedObjectives())
	{
		if (IsValid(ModifiedObjective))
		{
			ModifiedObjective->ResetObjective();
		}
	}
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
