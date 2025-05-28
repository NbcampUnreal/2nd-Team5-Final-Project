// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "UI/Struct/SLLevelWidgetDataRow.h"
#include "SLWidgetPrivateDataAsset.generated.h"


UCLASS()
class STILLLOADING_API USLMapListPrivateDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const FSLMapListImageData& GetMapImageByChapter(ESLChapterType ChapterType);
	const FSLMapListData& GetMapDataByChapter(ESLChapterType ChapterType);

private:
	UPROPERTY(EditAnywhere)
	TMap<ESLChapterType, FSLMapListImageData> MapImageMap;

	UPROPERTY(EditAnywhere)
	TMap<ESLChapterType, FSLMapListData> MapDataMap;
};

UCLASS()
class STILLLOADING_API USLInGamePrivateDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const FSLInGameImageData& GetInGameImageByChapter(ESLChapterType ChapterType);

private:
	UPROPERTY(EditAnywhere)
	TMap<ESLChapterType, FSLInGameImageData> InGameImageMap;
};