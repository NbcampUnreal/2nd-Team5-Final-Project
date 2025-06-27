// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLSoundSettingWidget.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"

const FName USLSoundSettingWidget::BgmTagIndex = "BgmTag";
const FName USLSoundSettingWidget::EffectTagIndex = "EffectTag";

void USLSoundSettingWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	Super::InitWidget(NewUISubsystem);

	BgmVolumeSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateBgmVolume);
	EffectVolumeSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateEffectVolume);
	
	OnUpdatedSettingValue();
}

void USLSoundSettingWidget::OnUpdatedSettingValue()
{
	Super::OnUpdatedSettingValue();

	CheckValidOfUserDataSubsystem();

	BgmVolumeSlider->SetValue(UserDataSubsystem->GetCurrentBgmVolume());
	EffectVolumeSlider->SetValue(UserDataSubsystem->GetCurrentEffectVolume());
}

void USLSoundSettingWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FText> OptionTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Option)
		{
			OptionTextMap = UITextPool->TextMap;
			break;
		}
	}

	if (OptionTextMap.Contains(BgmTagIndex))
	{
		BgmVolumeText->SetText(OptionTextMap[BgmTagIndex]);
	}

	if (OptionTextMap.Contains(EffectTagIndex))
	{
		EffectVolumeText->SetText(OptionTextMap[EffectTagIndex]);
	}
}

bool USLSoundSettingWidget::ApplySliderImage(FSliderStyle& SliderStyle)
{
	if (!Super::ApplySliderImage(SliderStyle))
	{
		return false;
	}

	BgmVolumeSlider->SetWidgetStyle(SliderStyle);
	EffectVolumeSlider->SetWidgetStyle(SliderStyle);

	return true;
}

void USLSoundSettingWidget::UpdateBgmVolume(float VolumeValue)
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetBgmVolume(VolumeValue);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLSoundSettingWidget::UpdateEffectVolume(float VolumeValue)
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetEffectVolume(VolumeValue);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLSoundSettingWidget::CheckValidOfUserDataSubsystem()
{
	if (IsValid(UserDataSubsystem))
	{
		return;
	}

	UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();
	checkf(IsValid(UserDataSubsystem), TEXT("User Data Subsystem is invalid"));
}
