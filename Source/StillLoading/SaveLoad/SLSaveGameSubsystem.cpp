// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoad/SLSaveGameSubsystem.h"

#include "GameMode/SLGameModeBase.h"
#include "Objective/SLObjectiveBase.h"
#include "Kismet/GameplayStatics.h"
#include "Objective/SLObjectiveDataSettings.h"
#include "Objective/SLObjectiveSubsystem.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SubSystem/SLLevelTransferSubsystem.h"
#include "SaveLoad/SLSaveGame.h"
#include "SubSystem/SLUserDataSettings.h"

void USLSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<USLUserDataSubsystem>();
    Collection.InitializeDependency<USLLevelTransferSubsystem>();
    Collection.InitializeDependency<USLObjectiveSubsystem>();
    Super::Initialize(Collection);
    
    LoadGameData();
}

void USLSaveGameSubsystem::Deinitialize()
{
    Super::Deinitialize();

    SaveUserData();
    UGameplayStatics::SaveGameToSlot(CurrentSaveData, SlotName, 0);
}

void USLSaveGameSubsystem::SaveGameData()
{
    check(CurrentSaveData);

    SaveChapterData();
    SaveObjectiveData();
    
    UGameplayStatics::SaveGameToSlot(CurrentSaveData, SlotName, 0);
}

void USLSaveGameSubsystem::LoadGameData()
{
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
        CurrentSaveData = Cast<USLSaveGame>(Loaded);
        SendWidgetData(false);
        SendChapterData();
        SendObjectiveData();
    }
    else
    {
        ResetGameData();
        SendWidgetData(true);
    }
}

void USLSaveGameSubsystem::ResetGameData()
{
    CurrentSaveData = NewObject<USLSaveGame>();
    LoadObjectiveDefaultData();
    UGameplayStatics::SaveGameToSlot(CurrentSaveData, SlotName, 0);
}

void USLSaveGameSubsystem::LoadObjectiveDefaultData()
{
    const USLObjectiveDataSettings* ObjectiveDataSettings = GetDefault<USLObjectiveDataSettings>();
    USLObjectiveSubsystem* ObjectiveSubsystem = GetGameInstance()->GetSubsystem<USLObjectiveSubsystem>();
    TMap<ESLChapterType, FSLObjectiveRuntimeData>& CachedObjectiveData = ObjectiveSubsystem->GetCachedObjectiveDataRef();
    
    CachedObjectiveData.Empty(5);
    CurrentSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Empty(5);
    
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
        CurrentSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Add(ChapterType, ObjectiveSaveData);
        CachedObjectiveData.Add(ChapterType, ObjectiveRuntimeData);
    }
}

void USLSaveGameSubsystem::SaveUserData()
{ 
    UGameInstance* GameInstance = GetGameInstance();

    check(GameInstance);

    USLUserDataSubsystem* UserDataSubSystem = GameInstance->GetSubsystem<USLUserDataSubsystem>();

    check(UserDataSubSystem);

    CurrentSaveData->UserSaveData.ActionKeyMap = UserDataSubSystem->GetActionKeyMap();
    CurrentSaveData->UserSaveData.LanguageType = UserDataSubSystem->GetCurrentLanguage();
    CurrentSaveData->UserSaveData.BgmVolume = UserDataSubSystem->GetCurrentBgmVolume();
    CurrentSaveData->UserSaveData.EffectVolume = UserDataSubSystem->GetCurrentEffectVolume();
    CurrentSaveData->UserSaveData.Brightness = UserDataSubSystem->GetCurrentBrightness();

    CurrentSaveData->UserSaveData.WindowMode = UserDataSubSystem->GetCurrentWindowMode();
    TPair<float,float> ScreenSize = UserDataSubSystem->GetCurrentScreenSize();
    CurrentSaveData->UserSaveData.ScreenWidth = ScreenSize.Key;
    CurrentSaveData->UserSaveData.ScreenHeight = ScreenSize.Value;

    CurrentSaveData->UserSaveData.ActionKeyMap = UserDataSubSystem->GetActionKeyMap();
    CurrentSaveData->UserSaveData.KeySet = UserDataSubSystem->GetKeySet();
    
    UE_LOG(LogTemp, Warning, TEXT("Save Widget Data"));
}

void USLSaveGameSubsystem::SendWidgetData(bool bIsFirstGame)
{
    USLUserDataSubsystem* UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();
    checkf(IsValid(UserDataSubsystem), TEXT("User Data Subsystem is invalid"));

    checkf(IsValid(CurrentSaveData), TEXT("Current Save Game is invalid"));

    if (bIsFirstGame)
    {
        UserDataSubsystem->ApplyDefaultUserData();
    }
    else
    {
        UserDataSubsystem->ApplyLoadedUserData(CurrentSaveData->UserSaveData);
    }
}

void USLSaveGameSubsystem::SaveChapterData()
{
    USLLevelTransferSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
    checkf(IsValid(LevelSubsystem), TEXT("Level Subsystem is invalid"));
    CurrentSaveData->CurrentChapter = LevelSubsystem->GetCurrentChapter();
}

void USLSaveGameSubsystem::SaveObjectiveData()
{
    USLObjectiveSubsystem* ObjectiveSubsystem = GetGameInstance()->GetSubsystem<USLObjectiveSubsystem>();
    checkf(IsValid(ObjectiveSubsystem), TEXT("Objective Subsystem is invalid"));
    checkf(IsValid(CurrentSaveData), TEXT("Current Save Game is invalid"));

    if (CurrentSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Num() == 0)
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

                if (FObjectiveSaveData* ChapterSaveDataPtr = CurrentSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Find(ChapterType))
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
    checkf(IsValid(CurrentSaveData), TEXT("Current Save Game is invalid"));
    LevelSubsystem->SetCurrentChapter(CurrentSaveData->CurrentChapter);
}

void USLSaveGameSubsystem::SendObjectiveData()
{
    USLObjectiveSubsystem* ObjectiveSubsystem = GetGameInstance()->GetSubsystem<USLObjectiveSubsystem>();
    checkf(IsValid(ObjectiveSubsystem), TEXT("Objective Subsystem is invalid"));
    checkf(IsValid(CurrentSaveData), TEXT("Current Save Game is invalid"));

    if (CurrentSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Num() == 0)
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
            const auto ChapterObjectiveSaveMap = CurrentSaveData->ObjectiveSaveData.ChapterObjectiveSaveDataMap.Find(ChapterType);
            if(ChapterObjectiveSaveMap == nullptr)
            {
                continue;
            }
            const ESLObjectiveState ObjectiveState = ChapterObjectiveSaveMap->ObjectiveSaveDataMap.FindRef(Name);
            Objective->SetObjectiveState(ObjectiveState);
        }
    }
}
