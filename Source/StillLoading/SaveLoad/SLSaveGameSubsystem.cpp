// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoad/SLSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SaveLoad/SLSaveGame.h"



void USLSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<USLUserDataSubsystem>();
    Super::Initialize(Collection);
    LoadGame();

}

void USLSaveGameSubsystem::Deinitialize()
{
    Super::Deinitialize();
    SaveGame();
}



void USLSaveGameSubsystem::SaveGame()
{

    check(CurrentSaveGame);

    SaveWidgetData();
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
        
        SendWidgetData();
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

const FWidgetSaveData& USLSaveGameSubsystem::GetCurrentWidgetDataByRef() const
{
    check(CurrentSaveGame);
    return CurrentSaveGame->WidgetSaveData;
}



void USLSaveGameSubsystem::SaveWidgetData()
{ 

    UGameInstance* GameInstance = GetGameInstance();

    check(GameInstance);

    USLUserDataSubsystem* UserDataSubSystem = GameInstance->GetSubsystem<USLUserDataSubsystem>();

    check(UserDataSubSystem);

    CurrentSaveGame->WidgetSaveData.ActionKeyMap = UserDataSubSystem->GetActionKeyMap();
    CurrentSaveGame->WidgetSaveData.LanguageType = UserDataSubSystem->GetCurrentLanguage();
    CurrentSaveGame->WidgetSaveData.BgmVolume = UserDataSubSystem->GetCurrentBgmVolume();
    CurrentSaveGame->WidgetSaveData.EffectVolume = UserDataSubSystem->GetCurrentEffectVolume();
    CurrentSaveGame->WidgetSaveData.Brightness = UserDataSubSystem->GetCurrentBrightness();

    CurrentSaveGame->WidgetSaveData.WindowMode = UserDataSubSystem->GetCurrentWindowMode();
    TPair<float,float> ScreenSize = UserDataSubSystem->GetCurrentScreenSize();
    CurrentSaveGame->WidgetSaveData.ScreenWidth = ScreenSize.Key;
    CurrentSaveGame->WidgetSaveData.ScreenHeight = ScreenSize.Value;

    CurrentSaveGame->WidgetSaveData.ActionKeyMap = UserDataSubSystem->GetActionKeyMap();
    CurrentSaveGame->WidgetSaveData.KeySet = UserDataSubSystem->GetKeySet();
   

    UE_LOG(LogTemp, Warning, TEXT("Save Widget Data"));
}

void USLSaveGameSubsystem::SendWidgetData()
{
    USLUserDataSubsystem* UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();
    checkf(IsValid(UserDataSubsystem), TEXT("User Data Subsystem is invalid"));

    checkf(IsValid(CurrentSaveGame), TEXT("Current Save Game is invalid"));
    UserDataSubsystem->ApplyLoadedUserData(CurrentSaveGame->WidgetSaveData);
}




