// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "SLSoundSettingWidget.generated.h"

class UTextBlock;
class USlider;
class USLUserDataSubsystem;

UCLASS()
class STILLLOADING_API USLSoundSettingWidget : public USLAdditiveWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;

protected:
	virtual void ApplyTextData() override;
	virtual bool ApplySliderImage(FSliderStyle& SliderStyle) override;

private:
	// Sound Volume Funtion
	UFUNCTION()
	void UpdateBgmVolume(float VolumeValue);

	UFUNCTION()
	void UpdateEffectVolume(float VolumeValue);
	//

	void CheckValidOfUserDataSubsystem();

private:
	// Sound Volume Setting
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> BgmVolumeSlider = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<USlider> EffectVolumeSlider = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> BgmVolumeText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> EffectVolumeText = nullptr;
	//

	UPROPERTY()
	TObjectPtr<USLUserDataSubsystem> UserDataSubsystem = nullptr;

	static const FName BgmTagIndex;
	static const FName EffectTagIndex;
};
