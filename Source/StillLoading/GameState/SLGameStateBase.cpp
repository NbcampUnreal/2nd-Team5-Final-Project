// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameStateBase.h"

ASLGameStateBase::ASLGameStateBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

TArray<USLGameSubjectBase*> ASLGameStateBase::GetGameSubjects() const
{
	return GameSubjects;
}

void ASLGameStateBase::AddGameSubject(USLGameSubjectBase* Subject)
{
	GameSubjects.Add(Subject);
}

void ASLGameStateBase::RemoveGameSubject(USLGameSubjectBase* Subject)
{
	GameSubjects.Remove(Subject);
}
