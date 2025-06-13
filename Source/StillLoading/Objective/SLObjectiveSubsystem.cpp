// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveSubsystem.h"

#include "SLObjectiveBase.h"
#include "SLObjectiveDataSettings.h"
#include "SubSystem/SLLevelTransferTypes.h"

USLObjectiveBase* USLObjectiveSubsystem::GetObjective(const ESLChapterType Chapter, const FName Name)
{
    const FSLObjectiveRuntimeData& ChapterDataAsset = CachedChapterObjectiveDataMap.FindRef(Chapter);
    USLObjectiveBase* Objective = ChapterDataAsset.ChapterObjectiveMap.FindRef(Name);
    return Objective;
}

bool USLObjectiveSubsystem::IsObjectiveCompleted(const ESLChapterType Chapter, const FName Name)
{
    const USLObjectiveBase* Objective = GetObjective(Chapter, Name);
    if (Objective == nullptr)
    {
        return false;
    }
    return Objective->GetObjectiveState() == ESLObjectiveState::Complete;
}

void USLObjectiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (ObjectiveDataSettings == nullptr)
    {
        ObjectiveDataSettings = GetDefault<USLObjectiveDataSettings>();
    }
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
        
        CachedChapterObjectiveDataMap.Add(ChapterType, ObjectiveRuntimeData);
    }
}
