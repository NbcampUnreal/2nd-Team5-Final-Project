// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLBaseMinigameCond.h"
#include "Minigame/System/SLMinigameCondSubsystem.h"
// Sets default values
ASLBaseMinigameCond::ASLBaseMinigameCond()
{

}

void ASLBaseMinigameCond::MinigameFailed()
{
	SendCondition(ESLMinigameResult::Fail);
}

void ASLBaseMinigameCond::MinigamePass()
{
	SendCondition(ESLMinigameResult::Pass);
}

// Called when the game starts or when spawned
void ASLBaseMinigameCond::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLBaseMinigameCond::InitCondition()
{
	GetMinigameCondSubsystem()->SetMinigameResult(AssignedMinigame, ESLMinigameResult::None);
}

void ASLBaseMinigameCond::SendCondition(ESLMinigameResult result)
{
	GetMinigameCondSubsystem()->SetMinigameResult(AssignedMinigame, result);
}

USLMinigameCondSubsystem* ASLBaseMinigameCond::GetMinigameCondSubsystem()
{
	UGameInstance* GI = GetGameInstance();
	checkf(IsValid(GI), TEXT("UGameInstance is nullptr"));
	USLMinigameCondSubsystem* Subsystem = GI->GetSubsystem<USLMinigameCondSubsystem>();
	checkf(IsValid(Subsystem), TEXT("USLMinigameCondSubsystem is nullptr"));
	return Subsystem;
}


