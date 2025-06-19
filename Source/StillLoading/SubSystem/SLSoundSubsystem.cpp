// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/SLSoundSubsystem.h"
#include "SubSystem/SLSoundSettings.h"
#include "SubSystem/SLLevelTransferSubsystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystem/DataAssets/SLSoundDataAsset.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

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
		UIAudioComp = UGameplayStatics::CreateSound2D(GetGameInstance(), SoundSource, 1.0f);
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
	SetBgmVolume(BgmVolume);
	SetEffectVolume(EffectVolume);

	CheckValidOfSoundDataAsset();
	USoundBase* SoundSource = SoundDataAsset->GetBgmSoundSource(SoundType);

	if (!IsValid(SoundSource))
	{
		return;
	}

	StopBgmSound();

	if (!IsValid(BgmAudioComp))
	{
		BgmAudioComp = UGameplayStatics::CreateSound2D(GetGameInstance(), SoundSource, 1.0f);
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

void USLSoundSubsystem::PlayBattleSound(EBattleSoundType SoundType, const FVector& Location)
{
	CheckValidOfSoundDataAsset();

	USoundBase* SoundSource = SoundDataAsset->GetBattleSoundSource(SoundType);

	if (!IsValid(SoundSource))
	{
		return;
	}
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundSource, Location, FRotator::ZeroRotator, 1.0f);
}

void USLSoundSubsystem::SetBgmVolume(float VolumeValue)
{
	BgmVolume = VolumeValue;
	CheckValidOfSoundMix();
	USoundClass* BGMSoundClass = SoundSettings->BGMSoundClass.LoadSynchronous();

	if (IsValid(BGMSoundClass))
	{
		UGameplayStatics::SetSoundMixClassOverride(this, SoundMix, BGMSoundClass, VolumeValue, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(this, SoundMix);
	}
}

void USLSoundSubsystem::SetEffectVolume(float VolumeValue)
{
	EffectVolume = VolumeValue;
	CheckValidOfSoundMix();
	USoundClass* SFXSoundClass = SoundSettings->EffectSoundClass.LoadSynchronous();

	if (IsValid(SFXSoundClass))
	{
		UGameplayStatics::SetSoundMixClassOverride(this, SoundMix, SFXSoundClass, VolumeValue, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(this, SoundMix);
	}
}

void USLSoundSubsystem::CheckValidOfSoundMix()
{
	if (IsValid(SoundMix))
	{
		return;
	}

	CheckValidOfSoundSettings();
	SoundMix = SoundSettings->SoundMix.LoadSynchronous();
	checkf(IsValid(SoundMix), TEXT("SoundMix is invalid"));
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
