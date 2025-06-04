// Fill out your copyright notice in the Description page of Project Settings.


#include "SLObjectiveSubsystem.h"

#include "SLObjectiveBase.h"
#include "SLObjectiveDataAsset.h"
#include "SLObjectiveDataSettings.h"
#include "SubSystem/SLLevelTransferTypes.h"

USLObjectiveBase* USLObjectiveSubsystem::GetObjective(const ESLChapterType Chapter, const FName Name)
{
    if (ObjectiveDataSettings == nullptr)
    {
        ObjectiveDataSettings = GetDefault<USLObjectiveDataSettings>();
    }

    if (ObjectiveDataSettings == nullptr)
    {
        return nullptr;
    }

    const TSoftObjectPtr<USLObjectiveDataAsset>* ChapterDataAssetPtr = ObjectiveDataSettings->ChapterObjectiveDataMap.Find(Chapter);
    
    if (ChapterDataAssetPtr == nullptr)
    {
        return nullptr;
    }

    const USLObjectiveDataAsset* ChapterDataAsset = ChapterDataAssetPtr->LoadSynchronous();
    
    if (ChapterDataAsset == nullptr)
    {
        return nullptr;
    }

    USLObjectiveBase* Objective = ChapterDataAsset->ChapterObjectiveMap.FindRef(Name);
    
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