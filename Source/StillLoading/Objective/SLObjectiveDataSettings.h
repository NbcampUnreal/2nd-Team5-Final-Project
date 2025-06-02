// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SLObjectiveDataSettings.generated.h"

class USLObjectiveDataAsset;
/**
 * 
 */
UCLASS(Config = ObjectiveSettings, DefaultConfig, meta = (DisplayName = "Objective Settings"))
class STILLLOADING_API USLObjectiveDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "챕터 0 게임 목표 데이터 에셋")
	TSoftObjectPtr<USLObjectiveDataAsset> ObjectiveDataAssetChapter0;
	UPROPERTY(EditAnywhere, Config, Category = "챕터 1 게임 목표 데이터 에셋")
	TSoftObjectPtr<USLObjectiveDataAsset> ObjectiveDataAssetChapter1;
	UPROPERTY(EditAnywhere, Config, Category = "챕터 2 게임 목표 데이터 에셋")
	TSoftObjectPtr<USLObjectiveDataAsset> ObjectiveDataAssetChapter2;
	UPROPERTY(EditAnywhere, Config, Category = "챕터 3 게임 목표 데이터 에셋")
	TSoftObjectPtr<USLObjectiveDataAsset> ObjectiveDataAssetChapter3;
	UPROPERTY(EditAnywhere, Config, Category = "챕터 4 게임 목표 데이터 에셋")
	TSoftObjectPtr<USLObjectiveDataAsset> ObjectiveDataAssetChapter4;
};
