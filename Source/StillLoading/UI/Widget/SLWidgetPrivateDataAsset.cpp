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

const FSLInGameImageData& USLInGamePrivateDataAsset::GetInGameImageByChapter(ESLChapterType ChapterType)
{
	checkf(InGameImageMap.Contains(ChapterType), TEXT("InGame Image Type Not Contains"));
	return InGameImageMap[ChapterType];
}
