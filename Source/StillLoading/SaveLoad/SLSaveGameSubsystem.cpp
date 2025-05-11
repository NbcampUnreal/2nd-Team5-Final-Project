// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoad/SLSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoad/SLSaveGame.h"



void USLSaveGameSubsystem::SaveGame()
{
    if (!CurrentSaveGame)
    {
        CurrentSaveGame = NewObject<USLSaveGame>();
        UE_LOG(LogTemp, Warning, TEXT("Create New Save Data"));
    }

    UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
    UE_LOG(LogTemp, Warning, TEXT("Save Data"));
}

void USLSaveGameSubsystem::LoadGame()
{
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
        CurrentSaveGame = Cast<USLSaveGame>(Loaded);
        UE_LOG(LogTemp, Warning, TEXT("Data Load Succeed"));

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Save file does not exist created a new one"));
        CurrentSaveGame = NewObject<USLSaveGame>();
        UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
      
    }
    

}

void USLSaveGameSubsystem::NewGame()
{
    CurrentSaveGame = NewObject<USLSaveGame>();
    UE_LOG(LogTemp, Warning, TEXT("Create New Save Data"));
    UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
    UE_LOG(LogTemp, Warning, TEXT("Save Data"));
}



