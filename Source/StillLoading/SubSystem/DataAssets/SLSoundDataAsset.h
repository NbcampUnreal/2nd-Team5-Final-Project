// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SubSystem/SLSoundTypes.h"
#include "UI/SLUITypes.h"
#include "SLSoundDataAsset.generated.h"

UCLASS()
class STILLLOADING_API USLSoundDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	USoundBase* GetUISoundSource(ESLUISoundType SoundType);
	USoundBase* GetBgmSoundSource(ESLBgmSoundType SoundType);

private:
	UPROPERTY(EditAnywhere)
	TMap<ESLUISoundType, USoundBase*> UISoundMap;

	UPROPERTY(EditAnywhere)
	TMap<ESLBgmSoundType, USoundBase*> BgmSoundMap;
};
