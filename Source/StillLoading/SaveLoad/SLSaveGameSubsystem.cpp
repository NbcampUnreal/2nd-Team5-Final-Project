// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoad/SLSaveGameSubsystem.h"

#include "SLSettingSaveGame.h"
#include "Objective/SLObjectiveBase.h"
#include "Kismet/GameplayStatics.h"
#include "Objective/SLObjectiveDataSettings.h"
#include "Objective/SLObjectiveSubsystem.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SubSystem/SLLevelTransferSubsystem.h"
#include "SaveLoad/SLSaveGame.h"

void USLSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<USLUserDataSubsystem>();
    Collection.InitializeDependency<USLLevelTransferSubsystem>();
    Collection.InitializeDependency<USLObjectiveSubsystem>();
    Super::Initialize(Collection);

    CheckSaveData();
    LoadSettingData();
    
    CurrentGameSlotName = "";
    CurrentGameSaveData = NewObject<USLSaveGame>();
    LoadObjectiveDefaultData();
}

void USLSaveGameSubsystem::Deinitialize()
{
    Super::Deinitialize();

    SaveSettingData();
}

void USLSaveGameSubsystem::SaveGameData()
{
    check(CurrentGameSaveData);

    SaveChapterData();
    SaveObjectiveData();
    
    UGameplayStatics::SaveGameToSlot(CurrentGameSaveData, CurrentGameSlotName, 0);
}

void USLSaveGameSubsystem::LoadGameData()
{
    if (UGameplayStatics::DoesSaveGameExist(CurrentGameSlotName, 0))
    {
        USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(CurrentGameSlotName, 0);
        CurrentGameSaveData = Cast<USLSaveGame>(Loaded);
        
        SendChapterData();
        SendObjectiveData();
    }
}

void USLSaveGameSubsystem::ResetGameData()
{
    CurrentGameSaveData = NewObject<USLSaveGame>();
    LoadObjectiveDefaultData();
    SendChapterData();
    StartSaveLoadTime = FPlatformTime::Seconds();
    //UGameplayStatics::SaveGameToSlot(CurrentGameSaveData, CurrentGameSlotName, 0);
}

void USLSaveGameSubsystem::LoadSettingData()
{
    if (UGameplayStatics::DoesSaveGameExist(SettingSlotName, 0))
    {
        USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SettingSlotName, 0);
        SettingSaveData = Cast<USLSettingSaveGame>(Loaded);
        bIsExistSaveData = true;
    }
    else
    {
        SettingSaveData = NewObject<USLSettingSaveGame>();
    }
    SendWidgetData();
}

TArray<FSlotSaveData> USLSaveGameSubsystem::GetSaveSlotList()
{
    TArray<FSlotSaveData> SaveSlotList;
    for (int32 Index = 0; Index < GameSaveSlotList.Num(); Index++)
    {
        USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(GameSaveSlotList[Index], 0);
        USLSaveGame* SaveGame = Cast<USLSaveGame>(Loaded);
        
        SaveSlotList.Add(SaveGame->SlotSaveData);
    }
    return SaveSlotList;
}

void USLSaveGameSubsystem::SaveGameDataByIndex(const int Index)
{
    check(GameSaveSlotList.IsValidIndex(Index));

    SaveChapterData();
    SaveObjectiveData();
    SaveSlotData();
    
    CurrentGameSaveData->SlotSaveData.SaveTime = FDateTime::Now();
    
    UGameplayStatics::SaveGameToSlot(CurrentGameSaveData, GameSaveSlotList[Index], 0);
}

const FSlotSaveData& USLSaveGameSubsystem::LoadGameDataByIndex(const int Index)
{
    check(GameSaveSlotList.IsValidIndex(Index));
    
    CurrentGameSlotName = GameSaveSlotList[Index];
    
    StartSaveLoadTime = FPlatformTime::Seconds();
    USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(CurrentGameSlotName, 0);
    CurrentGameSaveData = Cast<USLSaveGame>(Loaded);
    
    return CurrentGameSaveData->SlotSaveData;
}

void USLSaveGameSubsystem::OnSelectedNewGame()
{
    bIsExistSaveData = true;
}

bool USLSaveGameSubsystem::GetIsExistSaveData() const
{
    return bIsExistSaveData;
}

void USLSaveGameSubsystem::CheckSaveData()
{
    for (int32 Index = 0; Index < GameSaveSlotList.Num(); Index++)
    {
        if (!UGameplayStatics::DoesSaveGameExist(GameSaveSlotList[Index], 0))
        {
            CurrentGameSaveData = NewObject<USLSaveGame>();
            LoadObjectiveDefaultData();
            UGameplayStatics::SaveGameToSlot(CurrentGameSaveData, GameSaveSlotList[Index], 0);
        }
    }
}

void USLSaveGameSubsystem::LoadObjectiveDefaultData()
{
    const USLObjectiveDataSettings* ObjectiveDataSettings = GetDefault<USLObjectiveDataSettings>();
    USLObjectiveSubsystem* ObjectiveSubsystem = GetGameInstance()->GetSubsystem<USLObjectiveSubsystem>();
    TMap<ESLChapterType, FSLObjectiveRuntimeData>& CachedObjectiveData = ObjectiveSubsystem->GetCachedObjectiveDataRef();
    
    CachedObjectiveData.Empty(5);
    CurrentGameSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Empty(5);
    
    for (int32 Chapter = 1; Chapter <= 5; Chapter++)
    {
        const ESLChapterType ChapterType = static_cast<ESLChapterType>(Chapter);
        const TSoftObjectPtr<USLObjectiveDataAsset>* ChapterDataAssetPtr = ObjectiveDataSettings->ChapterObjectiveDataMap.Find(ChapterType);
        if (ChapterDataAssetPtr == nullptr)
        {
            continue;
        }
        const USLObjectiveDataAsset* ChapterDataAsset = ChapterDataAssetPtr->LoadSynchronous();
        FObjectiveSaveData ObjectiveSaveData;
        FSLObjectiveRuntimeData ObjectiveRuntimeData;
        
        for (auto&[Name, Objective] : ChapterDataAsset->ChapterObjectiveMap)
        {
            USLObjectiveBase* NewObjective = DuplicateObject(Objective, this);
            ObjectiveSaveData.ObjectiveSaveDataMap.Add(Name, Objective->GetObjectiveState());
            ObjectiveRuntimeData.ChapterObjectiveMap.Add(Name, NewObjective);
        }
        CurrentGameSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Add(ChapterType, ObjectiveSaveData);
        CachedObjectiveData.Add(ChapterType, ObjectiveRuntimeData);
    }
}

void USLSaveGameSubsystem::SaveSettingData()
{ 
    UGameInstance* GameInstance = GetGameInstance();

    check(GameInstance);

    USLUserDataSubsystem* UserDataSubSystem = GameInstance->GetSubsystem<USLUserDataSubsystem>();

    check(UserDataSubSystem);

    SettingSaveData->SettingSaveData.ActionKeyMap = UserDataSubSystem->GetActionKeyMap();
    SettingSaveData->SettingSaveData.LanguageType = UserDataSubSystem->GetCurrentLanguage();
    SettingSaveData->SettingSaveData.BgmVolume = UserDataSubSystem->GetCurrentBgmVolume();
    SettingSaveData->SettingSaveData.EffectVolume = UserDataSubSystem->GetCurrentEffectVolume();
    SettingSaveData->SettingSaveData.Brightness = UserDataSubSystem->GetCurrentBrightness();

    SettingSaveData->SettingSaveData.WindowMode = UserDataSubSystem->GetCurrentWindowMode();
    TPair<float,float> ScreenSize = UserDataSubSystem->GetCurrentScreenSize();
    SettingSaveData->SettingSaveData.ScreenWidth = ScreenSize.Key;
    SettingSaveData->SettingSaveData.ScreenHeight = ScreenSize.Value;

    SettingSaveData->SettingSaveData.ActionKeyMap = UserDataSubSystem->GetActionKeyMap();
    SettingSaveData->SettingSaveData.KeySet = UserDataSubSystem->GetKeySet();

    UGameplayStatics::SaveGameToSlot(SettingSaveData, SettingSlotName, 0);
}

void USLSaveGameSubsystem::SendWidgetData()
{
    USLUserDataSubsystem* UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();
    checkf(IsValid(UserDataSubsystem), TEXT("User Data Subsystem is invalid"));

    if (bIsExistSaveData)
    {
        checkf(IsValid(SettingSaveData), TEXT("Current Save Game is invalid"));
        UserDataSubsystem->ApplyLoadedUserData(SettingSaveData->SettingSaveData);
    }
    else
    {
        UserDataSubsystem->ApplyDefaultUserData();
    }
}

void USLSaveGameSubsystem::SaveChapterData()
{
    USLLevelTransferSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
    checkf(IsValid(LevelSubsystem), TEXT("Level Subsystem is invalid"));
    CurrentGameSaveData->SlotSaveData.ChapterSaveData = LevelSubsystem->GetCurrentChapter();
    CurrentGameSaveData->SlotSaveData.LevelSaveData = LevelSubsystem->GetCurrentLevelType();
}

void USLSaveGameSubsystem::SaveSlotData()
{
    USLLevelTransferSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
    checkf(IsValid(LevelSubsystem), TEXT("Level Subsystem is invalid"));
    CurrentGameSaveData->SlotSaveData.ChapterSaveData = LevelSubsystem->GetCurrentChapter();
    CurrentGameSaveData->SlotSaveData.LevelSaveData = LevelSubsystem->GetCurrentLevelType();
    
    const float DurationTime = FPlatformTime::Seconds() - StartSaveLoadTime;
    CurrentGameSaveData->SlotSaveData.PlayTimeInSeconds += DurationTime;
    CurrentGameSaveData->SlotSaveData.SaveTime = FDateTime::Now();
}

void USLSaveGameSubsystem::SaveObjectiveData()
{
    USLObjectiveSubsystem* ObjectiveSubsystem = GetGameInstance()->GetSubsystem<USLObjectiveSubsystem>();
    checkf(IsValid(ObjectiveSubsystem), TEXT("Objective Subsystem is invalid"));
    checkf(IsValid(CurrentGameSaveData), TEXT("Current Save Game is invalid"));

    if (CurrentGameSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Num() == 0)
    {
        LoadObjectiveDefaultData();
        return;
    }
    
    TMap<ESLChapterType, FSLObjectiveRuntimeData>& CachedObjectiveData = ObjectiveSubsystem->GetCachedObjectiveDataRef();
    
    for (int32 Chapter = 1; Chapter <= 5; Chapter++)
    {
        const ESLChapterType ChapterType = static_cast<ESLChapterType>(Chapter);
        
        if (FSLObjectiveRuntimeData* ChapterObjectiveDataPtr = CachedObjectiveData.Find(ChapterType))
        {
            FSLObjectiveRuntimeData& ChapterObjectiveData = *ChapterObjectiveDataPtr;

            for (auto& [Name, Objective] : ChapterObjectiveData.ChapterObjectiveMap)
            {
                if (!IsValid(Objective))
                {
                    continue;
                }

                if (FObjectiveSaveData* ChapterSaveDataPtr = CurrentGameSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Find(ChapterType))
                {
                    if (ESLObjectiveState* ObjectiveStatePtr = ChapterSaveDataPtr->ObjectiveSaveDataMap.Find(Name))
                    {
                        *ObjectiveStatePtr = Objective->GetObjectiveState();
                    }
                }
            }
        }
    }
}

void USLSaveGameSubsystem::SendChapterData()
{
    USLLevelTransferSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
    checkf(IsValid(LevelSubsystem), TEXT("Level Subsystem is invalid"));
    checkf(IsValid(CurrentGameSaveData), TEXT("Current Save Game is invalid"));
    LevelSubsystem->SetCurrentChapter(CurrentGameSaveData->SlotSaveData.ChapterSaveData);
}

void USLSaveGameSubsystem::SendObjectiveData()
{
    USLObjectiveSubsystem* ObjectiveSubsystem = GetGameInstance()->GetSubsystem<USLObjectiveSubsystem>();
    checkf(IsValid(ObjectiveSubsystem), TEXT("Objective Subsystem is invalid"));
    checkf(IsValid(CurrentGameSaveData), TEXT("Current Save Game is invalid"));

    if (CurrentGameSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Num() == 0)
    {
        LoadObjectiveDefaultData();
        return;
    }
    
    TMap<ESLChapterType, FSLObjectiveRuntimeData>& CachedObjectiveData = ObjectiveSubsystem->GetCachedObjectiveDataRef();
    for (int32 Chapter = 1; Chapter <= 5; Chapter++)
    {
        const ESLChapterType ChapterType = static_cast<ESLChapterType>(Chapter);
        FSLObjectiveRuntimeData& ChapterObjectiveData = *CachedObjectiveData.Find(ChapterType);

        for (auto&[Name, Objective] : ChapterObjectiveData.ChapterObjectiveMap)
        {
            const auto ChapterObjectiveSaveMap = CurrentGameSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Find(ChapterType);
            if(ChapterObjectiveSaveMap == nullptr)
            {
                continue;
            }
            const ESLObjectiveState ObjectiveState = ChapterObjectiveSaveMap->ObjectiveSaveDataMap.FindRef(Name);
            Objective->SetObjectiveState(ObjectiveState);
        }
    }
}
