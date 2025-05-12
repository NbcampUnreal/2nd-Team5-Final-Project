// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLOptionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ExpandableArea.h"
#include "Components/Slider.h"
#include "UI/SLUISubsystem.h"
#include "Animation/WidgetAnimation.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

const TArray<TPair<float, float>> USLOptionWidget::ResolutionSet = { {1920, 1080}, {1280, 800}, {1680, 1050} };

void USLOptionWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::EAW_OptionWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;
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
}

void USLOptionWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	if (IsValid(OpenAnim))
	{
		PlayAnimation(OpenAnim);
	}
	else
	{
		OnEndedOpenAnim();
	}

	UpdateScreenModeButton();
	UpdateLanguageButton();

	PlayUISound(ESLUISoundType::EUS_Open);
}

void USLOptionWidget::DeactivateWidget()
{
	if (IsValid(CloseAnim))
	{
		PlayAnimation(CloseAnim);
	}
	else
	{
		OnEndedCloseAnim();
	}

	PlayUISound(ESLUISoundType::EUS_Close);
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
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetLanguage(ESLLanguageType::EL_Kor);
	UpdateLanguageButton();
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedEng()
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetLanguage(ESLLanguageType::EL_Eng);
	UpdateLanguageButton();
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedFullScreen()
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetWindowMode(EWindowMode::Fullscreen);
	UpdateScreenModeButton();
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedWindowScreen()
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetWindowMode(EWindowMode::Windowed);
	UpdateScreenModeButton();
	PlayUISound(ESLUISoundType::EUS_Click);
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
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetBgmVolume(VolumeValue);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::UpdateEffectVolume(float VolumeValue)
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetEffectVolume(VolumeValue);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::UpdateBrightness(float BrightnessValue)
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetBrightness(BrightnessValue);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedKeySetting()
{
	CheckValidOfUISubsystem();
	UISubsystem->AddAdditveWidget(ESLAdditiveWidgetType::EAW_KeySettingWidget);
}

void USLOptionWidget::OnClickedQuit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void USLOptionWidget::CheckValidOfUserDataSubsystem()
{
	if (IsValid(UserDataSubsystem))
	{
		return;
	}

	UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();
	checkf(IsValid(UserDataSubsystem), TEXT("User Data Subsystem is invalid"));
}

void USLOptionWidget::UpdateScreenModeButton()
{
	CheckValidOfUserDataSubsystem();

	EWindowMode::Type CurrentWindowMode = (EWindowMode::Type)UserDataSubsystem->GetCurrentWindowMode();
	bool bIsFullScreen = false;

	if (CurrentWindowMode == EWindowMode::Fullscreen)
	{
		bIsFullScreen = true;
	}

	WindowScreenButton->SetIsEnabled(bIsFullScreen);
	FullScreenButton->SetIsEnabled(!bIsFullScreen);
	ResolutionList->SetIsEnabled(!bIsFullScreen);
}

void USLOptionWidget::UpdateLanguageButton()
{
	CheckValidOfUserDataSubsystem();
	
	ESLLanguageType CurrentLanguage = UserDataSubsystem->GetCurrentLanguage();
	bool bIsKor = false;

	if (CurrentLanguage == ESLLanguageType::EL_Kor)
	{
		bIsKor = true;
	}

	KorButton->SetIsEnabled(!bIsKor);
	EngButton->SetIsEnabled(bIsKor);
}

void USLOptionWidget::UpdateResolution(int32 ResolutionNum)
{
	int32 TargetIndex = ResolutionNum % 3;
	float Width = ResolutionSet[TargetIndex].Key;
	float Height = ResolutionSet[TargetIndex].Value;

	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetScreenSize(ResolutionSet[TargetIndex]);

	ResolutionList->SetIsExpanded(false);
	CurrentResolutionText->SetText(FText::FromString(FString::Printf(TEXT("%.0f * %.0f"), Width, Height)));
	PlayUISound(ESLUISoundType::EUS_Click);
}
