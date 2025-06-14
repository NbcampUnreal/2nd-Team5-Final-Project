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
	const TMap<ESLInGamePrivateImageType, TObjectPtr<UObject>>& GetBrushDataMap() const;

private:
	UPROPERTY(EditAnywhere)
	TMap<ESLInGamePrivateImageType, TObjectPtr<UObject>> InGameImageMap;
};

UCLASS()
class STILLLOADING_API USLTitlePrivateDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const TMap<ESLTitlePrivateImageType, TObjectPtr<UObject>>& GetBrushDataMap() const;

private:
	UPROPERTY(EditAnywhere)
	TMap<ESLTitlePrivateImageType, TObjectPtr<UObject>> TitleImageMap;
};

UCLASS()
class STILLLOADING_API USLOptionPrivateDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const TMap<ESLOptionPrivateImageType, TObjectPtr<UObject>>& GetBrushDataMap() const;

private:
	UPROPERTY(EditAnywhere)
	TMap<ESLOptionPrivateImageType, TObjectPtr<UObject>> OptionImageMap;
};