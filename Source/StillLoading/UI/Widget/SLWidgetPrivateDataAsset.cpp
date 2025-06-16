// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLWidgetPrivateDataAsset.h"

const FSLMapListImageData& USLMapListPrivateDataAsset::GetMapImageByChapter(ESLChapterType ChapterType)
{
	checkf(MapImageMap.Contains(ChapterType), TEXT("Map Image Type Not Contains"));
	return MapImageMap[ChapterType];
}

const FSLMapListData& USLMapListPrivateDataAsset::GetMapDataByChapter(ESLChapterType ChapterType)
{
	checkf(MapDataMap.Contains(ChapterType), TEXT("Map Data Type Not Contains"));
	return MapDataMap[ChapterType];
}

const TMap<ESLInGamePrivateImageType, TObjectPtr<UObject>>& USLInGamePrivateDataAsset::GetBrushDataMap() const
{
	return InGameImageMap;
}

const TMap<ESLInGameActivateType, bool>& USLInGamePrivateDataAsset::GetActivateUIMap() const
{
	return ActivateUIMap;
}

const TMap<ESLTitlePrivateImageType, TObjectPtr<UObject>>& USLTitlePrivateDataAsset::GetBrushDataMap() const
{
	return TitleImageMap;
}

ESLLevelNameType USLTitlePrivateDataAsset::GetStartLevelType() const
{
	return StartLevelType;
}

const TMap<ESLOptionPrivateImageType, TObjectPtr<UObject>>& USLOptionPrivateDataAsset::GetBrushDataMap() const
{
	return OptionImageMap;
}
