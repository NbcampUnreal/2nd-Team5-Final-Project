// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SLUITypes.h"
#include "SLLevelWidgetDataRow.generated.h"


USTRUCT(BlueprintType)
struct STILLLOADING_API FSLLevelWidgetDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ESLChapterType TargetChapter = ESLChapterType::EC_Intro;

	UPROPERTY(EditAnywhere)
	FSlateFontInfo FontInfo;

	UPROPERTY(EditAnywhere)
	TMap<FName, TSoftObjectPtr<UTexture2D>> ImageMap;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLMapListDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ESLGameMapType GameMapType = ESLGameMapType::EGM_None;

	UPROPERTY(EditAnywhere)
	FName MapName = "";

	UPROPERTY(EditAnywhere)
	FName ImageIndex = "";

	UPROPERTY(EditAnywhere)
	int32 TargetRow = 0;

	UPROPERTY(EditAnywhere)
	int32 TargetCol = 0;
};