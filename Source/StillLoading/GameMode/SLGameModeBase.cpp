// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameModeBase.h"

#include "GameState/SLGameStateBase.h"

ASLGameModeBase::ASLGameModeBase()
{
}

void ASLGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SLGameState = Cast<ASLGameStateBase>(GameState);
	checkf(SLGameState, TEXT("GameState is not ASLGameStateBase"));
}

void ASLGameModeBase::LoadGame()
{
	
}

void ASLGameModeBase::StartGameSubject(USLGameSubjectBase* Subject)
{
	SLGameState->AddGameSubject(Subject);
	OnGameSubjectStartedDelegate.Broadcast(Subject);
}

void ASLGameModeBase::EndGameSubject(USLGameSubjectBase* Subject)
{
	SLGameState->RemoveGameSubject(Subject);
	OnGameSubjectFinishedDelegate.Broadcast(Subject);
}
