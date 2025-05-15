// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/SLUITypes.h"
#include "SLUISettings.generated.h"

class USLBaseWidget;

UCLASS(Config = UISetting, DefaultConfig, meta = (DisplayName = "UI Subsystem Settings"))
class STILLLOADING_API USLUISettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "WidgetClass")
	TMap<ESLAdditiveWidgetType, TSoftClassPtr<USLBaseWidget>> WidgetClassMap;

	UPROPERTY(EditAnywhere, Config, Category = "WidgetData")
	TSoftObjectPtr<UDataTable> WidgetDataTable = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "SFXData")
	TMap<ESLUISoundType, TSoftObjectPtr<USoundBase>> WidgetSoundMap;

	UPROPERTY(EditAnywhere, Config, Category = "Test Var")
	int32 CurrentChapter = 0;
};
