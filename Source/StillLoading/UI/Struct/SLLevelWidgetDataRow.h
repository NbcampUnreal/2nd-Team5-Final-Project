// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SLUITypes.h"
#include "SLLevelWidgetDataRow.generated.h"

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLMapListDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName MapName = "";

	UPROPERTY(EditAnywhere)
	int32 TargetRow = 0;

	UPROPERTY(EditAnywhere)
	int32 TargetCol = 0;

	UPROPERTY(EditAnywhere)
	bool DefaultEnabled = true;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLMapListData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<ESLGameMapType, FSLMapListDataRow> DataMap;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLMapListImageData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<ESLGameMapType, UTexture2D*> MapImageMap;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLInGameImageData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<ESLInGamePrivateImageType, UTexture2D*> InGameImageMap;
};