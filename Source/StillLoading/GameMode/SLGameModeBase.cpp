// Fill out your copyright notice in the Description page of Project Settings.


#include "SLGameModeBase.h"

#include "GameState/SLGameStateBase.h"
#include "Kismet/GameplayStatics.h"

ASLGameModeBase::ASLGameModeBase()
{
	GameStateClass = ASLGameStateBase::StaticClass();
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