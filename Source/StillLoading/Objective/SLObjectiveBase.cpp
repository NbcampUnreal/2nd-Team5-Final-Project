// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveBase.h"

#include "GameMode/SLGameModeBase.h"

void USLObjectiveBase::AddObjectiveProgress(const int32 Count)
{
	ObjectiveProgressCount += Count;

	OnObjectiveProgressChanged.Broadcast(ObjectiveProgressCount);
	if (ObjectiveProgressCount >= ObjectiveCompleteCount && ObjectiveState == ESLObjectiveState::InProgress)
	{
		SetObjectiveState(ESLObjectiveState::Complete);
	}
}

void USLObjectiveBase::ObjectiveFail()
{
	ObjectiveState = ESLObjectiveState::Fail;
	OnObjectiveStateChanged.Broadcast(ObjectiveState);
}

void USLObjectiveBase::SetObjectiveState(const ESLObjectiveState InState)
{
	if (ObjectiveState == InState)
	{
		return;
	}
	
	ObjectiveState = InState;
	switch (InState)
	{
		default:
		break;
		case ESLObjectiveState::InProgress:
			GetWorld()->GetAuthGameMode<ASLGameModeBase>()->AddInProgressObjective(this);
			break;
		case ESLObjectiveState::Complete:
		case ESLObjectiveState::Fail:
			GetWorld()->GetAuthGameMode<ASLGameModeBase>()->RemoveInProgressObjective(this);
		break;
	}
	OnObjectiveStateChanged.Broadcast(ObjectiveState);
}

void USLObjectiveBase::SetObjectiveStateDelayed(const ESLObjectiveState InState, const float DelayTime)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, InState]
	{
		SetObjectiveState(InState);
	}, DelayTime, false);
}