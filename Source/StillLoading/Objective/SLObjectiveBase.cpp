// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveBase.h"

#include "GameMode/SLGameModeBase.h"
#include "SaveLoad/SLSaveDataStructs.h"

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

void USLObjectiveBase::ResetObjective()
{
	ObjectiveProgressCount = 0;
	ObjectiveState = ESLObjectiveState::None;
}

void USLObjectiveBase::SetObjectiveState(const ESLObjectiveState InState)
{
	ASLGameModeBase* GM = Cast<ASLGameModeBase>(GetWorld()->GetAuthGameMode());

	if (IsValid(GM))
	{
		GM->OnUpdatedObjectiveState(this, InState);
	}

	if (ObjectiveState == InState)
	{
		return;
	}
	
	ObjectiveState = InState;
	OnObjectiveStateChanged.Broadcast(ObjectiveState);
}

void USLObjectiveBase::SetObjectiveStateDelayed(const ESLObjectiveState InState)
{
	TWeakObjectPtr WeakThis = this;
    
	GetWorld()->GetTimerManager().SetTimerForNextTick([WeakThis, InState]
	{
		if (WeakThis.IsValid())
		{
			WeakThis->SetObjectiveState(InState);
		}
	});

}
