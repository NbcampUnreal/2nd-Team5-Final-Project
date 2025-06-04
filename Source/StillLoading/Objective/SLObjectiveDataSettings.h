// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SLObjectiveDataSettings.generated.h"

enum class ESLChapterType : uint8;
class USLObjectiveDataAsset;
/**
 * 
 */
UCLASS(Config = ObjectiveSettings, DefaultConfig, meta = (DisplayName = "Objective Settings"))
class STILLLOADING_API USLObjectiveDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "게임 목표 데이터 에셋")
	TMap<ESLChapterType, TSoftObjectPtr<USLObjectiveDataAsset>> ChapterObjectiveDataMap;
};
