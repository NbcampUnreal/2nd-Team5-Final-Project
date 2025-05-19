// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Struct/SLWidgetImageDataRow.h"
#include "UI/SLUITypes.h"
#include "SLWidgetImageDataAsset.generated.h"


UCLASS()
class STILLLOADING_API USLWidgetImageDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const TMap<ESLPublicWidgetImageType, UTexture2D*>& GetImageDataMap() const;
	const FSlateFontInfo& GetFondInfo() const;

private:
	UPROPERTY(EditAnywhere)
	TMap<ESLPublicWidgetImageType, UTexture2D*> PublicImageMap;

	UPROPERTY(EditAnywhere)
	FSlateFontInfo FontInfo;
};
