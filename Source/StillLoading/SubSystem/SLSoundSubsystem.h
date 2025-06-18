// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SubSystem/SLSoundTypes.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLSoundSubsystem.generated.h"

class USLSoundSettings;
class USLSoundDataAsset;
class USoundMix;

UCLASS()
class STILLLOADING_API USLSoundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void PlayUISound(ESLUISoundType SoundType);

	UFUNCTION(BlueprintCallable)
	void StopUISound();

	UFUNCTION(BlueprintCallable)
	void PlayBgmSound(ESLBgmSoundType SoundType);

	UFUNCTION(BlueprintCallable)
	void StopBgmSound();

	UFUNCTION(BlueprintCallable)
	void PlayBattleSound(EBattleSoundType SoundType, const FVector& Location);

	void SetBgmVolume(float VolumeValue);
	void SetEffectVolume(float VolumeValue);

private:
	void CheckValidOfSoundMix();
	void CheckValidOfSoundDataAsset();
	void CheckValidOfSoundSettings();

private:
	UPROPERTY()
	const USLSoundSettings* SoundSettings = nullptr;

	UPROPERTY()
	TObjectPtr<USoundMix> SoundMix = nullptr;

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
