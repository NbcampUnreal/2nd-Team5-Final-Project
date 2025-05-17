// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/DataAssets/SLSoundDataAsset.h"

USoundBase* USLSoundDataAsset::GetUISoundSource(ESLUISoundType SoundType)
{
	if (UISoundMap.Contains(SoundType) &&
		IsValid(UISoundMap[SoundType]))
	{
		return UISoundMap[SoundType];
	}

	return nullptr;
}

USoundBase* USLSoundDataAsset::GetBgmSoundSource(ESLBgmSoundType SoundType)
{
	if (BgmSoundMap.Contains(SoundType) &&
		IsValid(BgmSoundMap[SoundType]))
	{
		return BgmSoundMap[SoundType];
	}

	return nullptr;
}
