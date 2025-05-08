// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLBaseMinigameCond.h"
#include "Minigame/System/SLMinigameCondSubsystem.h"
// Sets default values
ASLBaseMinigameCond::ASLBaseMinigameCond()
{


}

// Called when the game starts or when spawned
void ASLBaseMinigameCond::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLBaseMinigameCond::InitCondition()
{
	bIsClear = false;
}

void ASLBaseMinigameCond::SendCondition(ESLMinigameResult result)
{
	UGameInstance* GI = GetGameInstance();
	checkf(IsValid(GI), TEXT("UGameInstance is nullptr"));
	USLMinigameCondSubsystem* Subsystem = GI->GetSubsystem<USLMinigameCondSubsystem>();
	checkf(IsValid(Subsystem), TEXT("USLMinigameCondSubsystem is nullptr"));
	Subsystem->SetMinigameResult(AssignedMinigame, result);
}


