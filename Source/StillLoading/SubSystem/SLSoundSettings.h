// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/SLUITypes.h"
#include "SLSoundSettings.generated.h"

class USLSoundDataAsset;

UCLASS(Config = SoundSetting, DefaultConfig, meta = (DisplayName = "Sound Subsystem Settings"))
class STILLLOADING_API USLSoundSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "SoundSourceData")
	TMap < ESLChapterType, TSoftObjectPtr<USLSoundDataAsset>> ChapterSoundMap;

	/*UPROPERTY(EditAnywhere, Config, Category = "SoundSourceData")
	TSoftObjectPtr<USLSoundDataAsset> Chapter0SoundData = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "SoundSourceData")
	TSoftObjectPtr<USLSoundDataAsset> Chapter1SoundData = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "SoundSourceData")
	TSoftObjectPtr<USLSoundDataAsset> Chapter2SoundData = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "SoundSourceData")
	TSoftObjectPtr<USLSoundDataAsset> Chapter3SoundData = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "SoundSourceData")
	TSoftObjectPtr<USLSoundDataAsset> Chapter4SoundData = nullptr;*/
};
