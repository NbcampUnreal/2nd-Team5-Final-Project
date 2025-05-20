// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLSoundSubsystem.h"
#include "SubSystem/SLSoundSettings.h"
#include "SubSystem/SLLevelTransferSubsystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystem/DataAssets/SLSoundDataAsset.h"

void USLSoundSubsystem::PlayUISound(ESLUISoundType SoundType)
{
	CheckValidOfSoundDataAsset();

	USoundBase* SoundSource = SoundDataAsset->GetUISoundSource(SoundType);

	if (!IsValid(SoundSource))
	{
		return;
	}

	StopUISound();

	if (!IsValid(UIAudioComp))
	{
		UIAudioComp = UGameplayStatics::CreateSound2D(GetGameInstance(), SoundSource, EffectVolume);
		UIAudioComp->bAutoDestroy = false;
	}
	
	UIAudioComp->SetSound(SoundSource);
	UIAudioComp->Play();
}

void USLSoundSubsystem::StopUISound()
{
	if (IsValid(UIAudioComp) && UIAudioComp->IsPlaying())
	{
		UIAudioComp->Stop();
	}
}

void USLSoundSubsystem::PlayBgmSound(ESLBgmSoundType SoundType)
{
	CheckValidOfSoundDataAsset();

	USoundBase* SoundSource = SoundDataAsset->GetBgmSoundSource(SoundType);

	if (!IsValid(SoundSource))
	{
		return;
	}

	StopBgmSound();

	if (!IsValid(BgmAudioComp))
	{
		BgmAudioComp = UGameplayStatics::CreateSound2D(GetGameInstance(), SoundSource, BgmVolume);
		BgmAudioComp->bAutoDestroy = false;
	}

	BgmAudioComp->SetSound(SoundSource);
	BgmAudioComp->Play();
}

void USLSoundSubsystem::StopBgmSound()
{
	if (IsValid(BgmAudioComp))
	{
		BgmAudioComp->Stop();
	}
}

void USLSoundSubsystem::SetBgmVolume(float VolumeValue)
{
	BgmVolume = FMath::Clamp(VolumeValue, 0.0f, 1.0f);

	if (IsValid(BgmAudioComp))
	{
		BgmAudioComp->SetVolumeMultiplier(BgmVolume);

		if (FMath::IsNearlyZero(BgmVolume))
		{
			BgmAudioComp->SetPaused(true);
			return;
		}

		if (BgmAudioComp->bIsPaused)
		{
			BgmAudioComp->SetPaused(false);
		}
	}
}

void USLSoundSubsystem::SetEffectVolume(float VolumeValue)
{
	EffectVolume = FMath::Clamp(VolumeValue, 0.0f, 1.0f);

	if (IsValid(UIAudioComp))
	{
		UIAudioComp->SetVolumeMultiplier(EffectVolume);

		if (FMath::IsNearlyZero(EffectVolume))
		{
			UIAudioComp->SetPaused(true);
			return;
		}

		if (UIAudioComp->bIsPaused)
		{
			UIAudioComp->SetPaused(false);
		}
	}
}

void USLSoundSubsystem::CheckValidOfSoundDataAsset()
{
	USLLevelTransferSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
	checkf(IsValid(LevelSubsystem), TEXT("Level Subsystem is invalid"));

	ESLChapterType CurrentChapter = LevelSubsystem->GetCurrentChapter();

	if (PossessChapter == CurrentChapter &&
		IsValid(SoundDataAsset))
	{
		return;
	}

	CheckValidOfSoundSettings();
	checkf(SoundSettings->ChapterSoundMap.Contains(CurrentChapter), TEXT("Sound Map is not contains CurrentChapter Sound Data"));
	SoundDataAsset = SoundSettings->ChapterSoundMap[CurrentChapter].LoadSynchronous();
	PossessChapter = CurrentChapter;
}

void USLSoundSubsystem::CheckValidOfSoundSettings()
{
	if (IsValid(SoundSettings))
	{
		return;
	}

	SoundSettings = GetDefault<USLSoundSettings>();
	checkf(IsValid(SoundSettings), TEXT("Sound Settings is invalid"));
}
