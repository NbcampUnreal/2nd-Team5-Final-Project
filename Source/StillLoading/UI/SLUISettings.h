// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLUISettings.generated.h"

class USLAdditiveWidget;

UCLASS(Config = UISetting, DefaultConfig, meta = (DisplayName = "UI Subsystem Settings"))
class STILLLOADING_API USLUISettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "WidgetClass")
	TMap<ESLAdditiveWidgetType, TSoftClassPtr<USLAdditiveWidget>> WidgetClassMap;

	UPROPERTY(EditAnywhere, Config, Category = "WidgetData")
	TMap<ESLChapterType, TSoftObjectPtr<UDataAsset>> ChapterWidgetPublicDataMap;

	UPROPERTY(EditAnywhere, Config, Category = "WidgetData")
	TMap<ESLChapterType, TSoftObjectPtr<UDataAsset>> OptionWidgetPrivateDataMap;
};
