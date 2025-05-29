// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLWidgetImageDataAsset.h"

const FSlateFontInfo& USLWidgetImageDataAsset::GetFondInfo() const
{
	return FontInfo;
}

const TMap<ESLPublicWidgetImageType, UObject*>& USLWidgetImageDataAsset::GetBrushDataMap() const
{
	return PublicBrushMap;
}
