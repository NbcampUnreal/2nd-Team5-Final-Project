// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SubSystem/SLSoundTypes.h"
#include "SLLevelTransferSettings.generated.h"

class USLLevelDataAsset;

UCLASS(Config = LevelSetting, DefaultConfig, meta = (DisplayName = "Level Trsnsfer Subsystem Settings"))
class STILLLOADING_API USLLevelTransferSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "LevelTransferData")
	TMap<ESLChapterType, TSoftObjectPtr<USLLevelDataAsset>> ChapterLevelDataMap;

	UPROPERTY(EditAnywhere, Config, Category = "LevelBgmData")
	TMap<ESLLevelNameType, ESLBgmSoundType> LevelBgmMap;
};
