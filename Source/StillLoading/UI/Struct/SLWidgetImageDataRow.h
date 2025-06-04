// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLWidgetImageDataRow.generated.h"


USTRUCT(BlueprintType)
struct STILLLOADING_API FSLWidgetImageDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	ESLAdditiveWidgetType WidgetType = ESLAdditiveWidgetType::EAW_OptionWidget;

	UPROPERTY(EditAnywhere)
	ESLChapterType TargetChapter = ESLChapterType::EC_Chapter0;

	UPROPERTY(EditAnywhere)
	FSlateFontInfo FontInfo;

	UPROPERTY(EditAnywhere)
	TMap<FName, TSoftObjectPtr<UTexture2D>> ImageMap;
};