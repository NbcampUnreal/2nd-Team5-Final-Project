// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLOptionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ExpandableArea.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "UI/SLUISubsystem.h"
#include "Engine/RendererSettings.h"

const TArray<TPair<float, float>> USLOptionWidget::ResolutionSet = { {1920, 1080}, {1280, 800}, {1680, 1050} };

void USLOptionWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::OptionWidget;
	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem, ChapterType);

	FullScreenButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedFullScreen);
	WindowScreenButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedWindowScreen);

	KorButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedKor);
	EngButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedEng);

	FirstResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedFirstResolution);
	SecondResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedSecondResolution);
	ThirdResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedThirdResolution);

	BgmVolumeSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateBgmVolume);
	EffectVolumeSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateEffectVolume);

	BrightnessSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateBrightness);

	KeySettingButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedKeySetting);
	QuitGameButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuit);
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);

	if (IsValid(KeySettingPanel))
	{
		KeySettingPanel->InitWidget(UISubsystem, CurrentChapter);
	}
}

void USLOptionWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	if (CheckValidOfGameUserSettings())
	{
		if (GameUserSettings->GetFullscreenMode() == EWindowMode::Fullscreen)
		{
			bIsFullScreen = true;
		}
		else
		{
			bIsFullScreen = false;
		}

		UpdateScreenModeButton();
	}

	UpdateLanguageButton();

	if (IsValid(KeySettingPanel))
	{
		KeySettingPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USLOptionWidget::ApplyImageData()
{
	Super::ApplyImageData();


}

void USLOptionWidget::ApplyFontData()
{
	Super::ApplyFontData();

}

void USLOptionWidget::OnClickedKor()
{
	bIsKor = true;
	UpdateLanguageButton();
	PlayUISound(ESLUISoundType::Click);

	if (CheckValidOfUISubsystem())
	{
		UISubsystem->SetLanguageToUI(ESLLanguageType::Kor);
	}
}

void USLOptionWidget::OnClickedEng()
{
	bIsKor = false;
	UpdateLanguageButton();
	PlayUISound(ESLUISoundType::Click);

	if (CheckValidOfUISubsystem())
	{
		UISubsystem->SetLanguageToUI(ESLLanguageType::Eng);
	}
}

void USLOptionWidget::OnClickedFullScreen()
{
	bIsFullScreen = true;
	UpdateScreenModeButton();
	PlayUISound(ESLUISoundType::Click);

	if (!CheckValidOfGameUserSettings())
	{
		return;
	}

	GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	GameUserSettings->ApplySettings(false);
}

void USLOptionWidget::OnClickedWindowScreen()
{
	bIsFullScreen = false;
	UpdateScreenModeButton();
	PlayUISound(ESLUISoundType::Click);

	if (!CheckValidOfGameUserSettings())
	{
		return;
	}

	GameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
	GameUserSettings->ApplySettings(false);
}

void USLOptionWidget::OnClickedFirstResolution()
{
	UpdateResolution(0);
}

void USLOptionWidget::OnClickedSecondResolution()
{
	UpdateResolution(1);
}

void USLOptionWidget::OnClickedThirdResolution()
{
	UpdateResolution(2);
}

void USLOptionWidget::UpdateBgmVolume(float VolumeValue)
{
	// TODO : Send Bgm Sound Volume Value

	PlayUISound(ESLUISoundType::Click);
}

void USLOptionWidget::UpdateEffectVolume(float VolumeValue)
{
	// TODO : Send Effect Sound Volume Value

	if (!CheckValidOfUISubsystem())
	{
		return;
	}

	UISubsystem->SetEffectVolume(VolumeValue);
	PlayUISound(ESLUISoundType::Click);
}

void USLOptionWidget::UpdateBrightness(float BrightnessValue)
{
	if (!CheckValidOfRendererSettings())
	{
		return;
	}

	RendererSettings->DefaultFeatureAutoExposureBias = BrightnessValue;
	
	FPropertyChangedEvent PropertyChangedEvent(URendererSettings::StaticClass()->
		FindPropertyByName(GET_MEMBER_NAME_CHECKED(URendererSettings, DefaultFeatureAutoExposureBias)));

	RendererSettings->PostEditChangeProperty(PropertyChangedEvent);
	RendererSettings->SaveConfig();

	PlayUISound(ESLUISoundType::Click);
}

void USLOptionWidget::OnClickedKeySetting()
{
	if (IsValid(KeySettingPanel))
	{
		KeySettingPanel->SetVisibility(ESlateVisibility::Visible);
		KeySettingPanel->ActivateWidget(CurrentChapter);
	}
}

void USLOptionWidget::OnClickedQuit()
{
	// TODO : Quit Game
}

bool USLOptionWidget::CheckValidOfGameUserSettings()
{
	if (IsValid(GameUserSettings))
	{
		return true;
	}

	GameUserSettings = GEngine->GetGameUserSettings();

	if (!IsValid(GameUserSettings))
	{
		return false;
	}

	return true;
}

bool USLOptionWidget::CheckValidOfRendererSettings()
{
	if (IsValid(RendererSettings))
	{
		return true;
	}

	RendererSettings = GetMutableDefault<URendererSettings>();

	if (!IsValid(RendererSettings))
	{
		return false;
	}

	return true;
}

void USLOptionWidget::UpdateScreenModeButton()
{
	FullScreenButton->SetIsEnabled(!bIsFullScreen);
	WindowScreenButton->SetIsEnabled(bIsFullScreen);

	ResolutionList->SetIsEnabled(!bIsFullScreen);
}

void USLOptionWidget::UpdateLanguageButton()
{
	KorButton->SetIsEnabled(!bIsKor);
	EngButton->SetIsEnabled(bIsKor);
}

void USLOptionWidget::UpdateResolution(int32 ResolutionNum)
{
	int32 TargetIndex = ResolutionNum % 3;
	float Width = ResolutionSet[TargetIndex].Key;
	float Height = ResolutionSet[TargetIndex].Value;

	if (CheckValidOfGameUserSettings())
	{
		GameUserSettings->SetScreenResolution(FIntPoint(Width, Height));
		GameUserSettings->ApplySettings(false);
	}

	ResolutionList->SetIsExpanded(false);
	CurrentResolutionText->SetText(FText::FromString(FString::Printf(TEXT("%.0f * %.0f"), Width, Height)));
	PlayUISound(ESLUISoundType::Click);
}
