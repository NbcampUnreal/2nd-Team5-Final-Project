// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SubSystem/SLSoundTypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLSoundSubsystem.generated.h"

class USLSoundSettings;
class USLSoundDataAsset;

UCLASS()
class STILLLOADING_API USLSoundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void PlayUISound(ESLUISoundType SoundType);
	void StopUISound();

	void PlayBgmSound(ESLBgmSoundType SoundType);
	void StopBgmSound();

	void PlayBattleSound(EBattleSoundType SoundType, const FVector& Location);

	void SetBgmVolume(float VolumeValue);
	void SetEffectVolume(float VolumeValue);

private:
	void CheckValidOfSoundDataAsset();
	void CheckValidOfSoundSettings();

private:
	UPROPERTY()
	const USLSoundSettings* SoundSettings = nullptr;

	UPROPERTY()
	TObjectPtr<USLSoundDataAsset> SoundDataAsset = nullptr;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BgmAudioComp = nullptr;

	UPROPERTY()
	TObjectPtr<UAudioComponent> UIAudioComp = nullptr;

	ESLChapterType PossessChapter = ESLChapterType::EC_None;

	float BgmVolume = 0.0f;
	float EffectVolume = 0.0f;
};
