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

    TSoftObjectPtr<USLObjectiveDataAsset> ChapterDataAssetPtr;
    switch (Chapter)
    {
    case ESLChapterType::EC_Intro:
        ChapterDataAssetPtr = ObjectiveDataSettings->ObjectiveDataAssetChapter0;
        break;
    case ESLChapterType::EC_Chapter2D:
        ChapterDataAssetPtr = ObjectiveDataSettings->ObjectiveDataAssetChapter1;
        break;
    case ESLChapterType::EC_Chapter2_5D:
        ChapterDataAssetPtr = ObjectiveDataSettings->ObjectiveDataAssetChapter2;
        break;
    case ESLChapterType::EC_Chapter3D:
        ChapterDataAssetPtr = ObjectiveDataSettings->ObjectiveDataAssetChapter3;
        break;
    case ESLChapterType::EC_ChapterHighQuality:
        ChapterDataAssetPtr = ObjectiveDataSettings->ObjectiveDataAssetChapter4;
        break;
    default:
        return nullptr;
    }
    
    if (ChapterDataAssetPtr.IsNull())
    {
        return nullptr;
    }

    const USLObjectiveDataAsset* ChapterDataAsset = ChapterDataAssetPtr.LoadSynchronous();
    
    if (ChapterDataAsset == nullptr)
    {
        return nullptr;
    }

    for (const auto& Objective : ChapterDataAsset->ObjectiveList)
    {
        if (Objective && Objective->GetObjectiveName() == Name)
        {
            return Objective;
        }
    }

    return nullptr;
}
