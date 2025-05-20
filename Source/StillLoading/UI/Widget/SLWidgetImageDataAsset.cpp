// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLWidgetImageDataAsset.h"

const TMap<ESLPublicWidgetImageType, UTexture2D*>& USLWidgetImageDataAsset::GetImageDataMap() const
{
	return PublicImageMap;
}

const FSlateFontInfo& USLWidgetImageDataAsset::GetFondInfo() const
{
	return FontInfo;
}
