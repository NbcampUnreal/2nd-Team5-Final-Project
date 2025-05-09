// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoad/SLSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoad/SLSaveGame.h"

void USLSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void USLSaveGameSubsystem::SaveGameToSlot(FName SlotName)
{
    if (!CurrentSaveGame)
    {
        CurrentSaveGame = NewObject<USLSaveGame>();
    }

    UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName.ToString(), 0);
    CurrentSlotName = SlotName;
}

void USLSaveGameSubsystem::LoadGameFromSlot(FName SlotName)
{
    USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName.ToString(), 0);
    CurrentSaveGame = Cast<USLSaveGame>(Loaded);

    if (!CurrentSaveGame)
    {
        CurrentSaveGame = NewObject<USLSaveGame>();
    }

    CurrentSlotName = SlotName;
}
