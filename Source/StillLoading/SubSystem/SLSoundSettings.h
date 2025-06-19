// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLSoundSettings.generated.h"

class USLSoundDataAsset;
class USoundMix;

UCLASS(Config = SoundSetting, DefaultConfig, meta = (DisplayName = "Sound Subsystem Settings"))
class STILLLOADING_API USLSoundSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "SoundSourceData")
	TMap <ESLChapterType, TSoftObjectPtr<USLSoundDataAsset>> ChapterSoundMap;

	UPROPERTY(EditAnywhere, Config, Category = "SoundClass")
	TSoftObjectPtr<USoundMix> SoundMix = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "SoundClass")
	TSoftObjectPtr<USoundClass> BGMSoundClass = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "SoundClass")
	TSoftObjectPtr<USoundClass> EffectSoundClass = nullptr;
};
