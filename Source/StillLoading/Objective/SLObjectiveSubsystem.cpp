// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveSubsystem.h"

#include "SLObjectiveBase.h"
#include "SLObjectiveDataSettings.h"
#include "SaveLoad/SLSaveGameSubsystem.h"
#include "SubSystem/SLLevelTransferTypes.h"

USLObjectiveBase* USLObjectiveSubsystem::GetObjective(const ESLChapterType Chapter, const FName Name)
{
    const FSLObjectiveRuntimeData& ChapterDataAsset = CachedObjectiveData.FindRef(Chapter);
    USLObjectiveBase* Objective = ChapterDataAsset.ChapterObjectiveMap.FindRef(Name);
    return Objective;
}

ESLObjectiveState USLObjectiveSubsystem::GetObjectiveState(const ESLChapterType Chapter, const FName Name)
{
    if (const USLObjectiveBase* Objective = GetObjective(Chapter, Name))
    {
        return Objective->GetObjectiveState();
    }

    return ESLObjectiveState::None;
}

TArray<USLObjectiveBase*> USLObjectiveSubsystem::GetInProgressedObjectives()
{
    TArray<USLObjectiveBase*> InProgressedObjectives;
    for (const auto&[Chapter, ChapterData] : CachedObjectiveData)
    {
        for (const auto&[Name, Objective] : ChapterData.ChapterObjectiveMap)
        {
            if (Objective->GetObjectiveState() == ESLObjectiveState::InProgress)
            {
                InProgressedObjectives.Add(Objective);
            }
        }
    }
    return InProgressedObjectives;
}

TMap<ESLChapterType, FSLObjectiveRuntimeData>& USLObjectiveSubsystem::GetCachedObjectiveDataRef()
{
    return CachedObjectiveData;
}

void USLObjectiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    const USLObjectiveDataSettings* ObjectiveDataSettings = GetDefault<USLObjectiveDataSettings>();
    if (ObjectiveDataSettings == nullptr)
    {
        return;
    }
    
    for (int32 Chapter = 1; Chapter <= 5; Chapter++)
    {
        const ESLChapterType ChapterType = static_cast<ESLChapterType>(Chapter);
        const TSoftObjectPtr<USLObjectiveDataAsset>* ChapterDataAssetPtr = ObjectiveDataSettings->ChapterObjectiveDataMap.Find(ChapterType);
        if (ChapterDataAssetPtr == nullptr)
        {
            continue;
        }
        const USLObjectiveDataAsset* ChapterDataAsset = ChapterDataAssetPtr->LoadSynchronous();
        FSLObjectiveRuntimeData ObjectiveRuntimeData;
        
        for (auto&[Name, Objective] : ChapterDataAsset->ChapterObjectiveMap)
        {
            USLObjectiveBase* NewObjectiveBase = DuplicateObject(Objective, this);
            ObjectiveRuntimeData.ChapterObjectiveMap.Add(Name, NewObjectiveBase);
        }
        
        CachedObjectiveData.Add(ChapterType, ObjectiveRuntimeData);
    }
}
