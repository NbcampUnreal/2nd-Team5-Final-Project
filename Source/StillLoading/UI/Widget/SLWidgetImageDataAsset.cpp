// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLWidgetImageDataAsset.h"

const FSLPublicWidgetImageData& USLWidgetImageDataAsset::GetImageDataByChapter(ESLChapterType ChapterType) const
{
	checkf(ChapterPublicImageMap.Contains(ChapterType), TEXT("Chapter Type not contains"));
	return ChapterPublicImageMap[ChapterType];
}

const FSlateFontInfo& USLWidgetImageDataAsset::GetFondInfoByChapter(ESLChapterType ChapterType) const
{
	checkf(ChapterFontMap.Contains(ChapterType), TEXT("Chapter Type not contains"));
	return ChapterFontMap[ChapterType];
}
