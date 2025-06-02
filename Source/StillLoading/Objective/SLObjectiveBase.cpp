// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveBase.h"

void USLObjectiveBase::AddObjectiveProgress(const int32 Count)
{
	ObjectiveProgressCount += Count;
	if (ObjectiveProgressCount >= ObjectiveCompleteCount && ObjectiveState == ESLObjectiveState::InProgress)
	{
		ObjectiveComplete();
	}
}

void USLObjectiveBase::ObjectiveFail()
{
	ObjectiveState = ESLObjectiveState::Fail;
	OnObjectiveStateChanged.Broadcast(ObjectiveState);
}

void USLObjectiveBase::SetObjectiveState(const ESLObjectiveState InState)
{
	ObjectiveState = InState;
	OnObjectiveStateChanged.Broadcast(ObjectiveState);
}

void USLObjectiveBase::ObjectiveComplete()
{
	ObjectiveState = ESLObjectiveState::Complete;
	OnObjectiveStateChanged.Broadcast(ObjectiveState);
}
