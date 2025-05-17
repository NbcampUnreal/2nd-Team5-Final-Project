// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLOptionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ExpandableArea.h"
#include "Components/Slider.h"
#include "Components/Image.h"
#include "UI/SLUISubsystem.h"
#include "Animation/WidgetAnimation.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "Kismet/GameplayStatics.h"

const TArray<TPair<float, float>> USLOptionWidget::ResolutionSet = { {1920, 1080}, {1280, 800}, {1680, 1050} };

const FName USLOptionWidget::TitleTextIndex = "TitleText";
const FName USLOptionWidget::LanguageTagIndex = "LanguageTag";
const FName USLOptionWidget::WindowModeTagIndex = "WindowModeTag";
const FName USLOptionWidget::ResolutionTagIndex = "ResolutionTag";
const FName USLOptionWidget::BgmTagIndex = "BgmTag";
const FName USLOptionWidget::EffectTagIndex = "EffectTag";
const FName USLOptionWidget::BrigthnessTagIndex = "BrigthnessTag";
const FName USLOptionWidget::KorButtonIndex = "KorButton";
const FName USLOptionWidget::EngButtonIndex = "EngButton";
const FName USLOptionWidget::FullScreenButtonIndex = "FullScreenButton";
const FName USLOptionWidget::WindowedButtonIndex = "WindowedButton";
const FName USLOptionWidget::KeySettingButtonIndex = "KeySettingButton";
const FName USLOptionWidget::QuitGameButtonIndex = "QuitGameButton";
const FName USLOptionWidget::CloseButtonIndex = "CloseButton";

void USLOptionWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_OptionWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 15;
	bIsVisibleCursor = true;
	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem);

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

	InitOptionVariable();
}

void USLOptionWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

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

void USLOptionWidget::ApplyFontData()
{
	Super::ApplyFontData();

}

void USLOptionWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FSLUITextData> OptionTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Option)
		{
			OptionTextMap = UITextPool->TextMap;
			break;
		}
	}

	TitleText->SetText(OptionTextMap[TitleTextIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	KorText->SetText(OptionTextMap[KorButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	EngText->SetText(OptionTextMap[EngButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	LanguageModeText->SetText(OptionTextMap[LanguageTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	WindowModeText->SetText(OptionTextMap[WindowModeTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	FullScreenText->SetText(OptionTextMap[FullScreenButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	WindowScreenText->SetText(OptionTextMap[WindowedButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	ResolutionText->SetText(OptionTextMap[ResolutionTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	BgmVolumeText->SetText(OptionTextMap[BgmTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	EffectVolumeText->SetText(OptionTextMap[EffectTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	BrightnessText->SetText(OptionTextMap[BrigthnessTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	KeySettingText->SetText(OptionTextMap[KeySettingButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	QuitGameText->SetText(OptionTextMap[QuitGameButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	CloseText->SetText(OptionTextMap[CloseButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
}

bool USLOptionWidget::ApplyBackgroundImage()
{
	if (!Super::ApplyBackgroundImage())
	{
		return false;
	}

	BackgroundImg->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_Background]);

	return true;
}

bool USLOptionWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyButtonImage(ButtonStyle))
	{
		return false;
	}

	KorButton->SetStyle(ButtonStyle);
	EngButton->SetStyle(ButtonStyle);
	FullScreenButton->SetStyle(ButtonStyle);
	WindowScreenButton->SetStyle(ButtonStyle);
	KeySettingButton->SetStyle(ButtonStyle);
	QuitGameButton->SetStyle(ButtonStyle);
	CloseButton->SetStyle(ButtonStyle);

	FirstResolutionButton->SetStyle(ButtonStyle);
	SecondResolutionButton->SetStyle(ButtonStyle);
	ThirdResolutionButton->SetStyle(ButtonStyle);

	return true;
}

bool USLOptionWidget::ApplySliderImage(FSliderStyle& SliderStyle)
{
	if (!Super::ApplySliderImage(SliderStyle))
	{
		return false;
	}

	BgmVolumeSlider->SetWidgetStyle(SliderStyle);
	EffectVolumeSlider->SetWidgetStyle(SliderStyle);
	BrightnessSlider->SetWidgetStyle(SliderStyle);

	return true;
}

bool USLOptionWidget::ApplyBorderImage()
{
	if (!Super::ApplyBorderImage())
	{
		return false;
	}

	OptionPanelBack->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_NormalBorder]);

	return true;
}

bool USLOptionWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	if (PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_Expandable) &&
		IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_Expandable]))
	{
		FSlateBrush SlateBrush;

		SlateBrush.SetResourceObject(PublicImageMap[ESLPublicWidgetImageType::EPWI_Expandable]);
		ResolutionList->SetBorderBrush(SlateBrush);
		LanguageList->SetBorderBrush(SlateBrush);
	}

	if (PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_Expanded) &&
		PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_Collapsed) &&
		IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_Expanded]) &&
		IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_Collapsed]))
	{
		FExpandableAreaStyle ExpandableAreaStyle;
		FSlateBrush SlateBrush;
		
		SlateBrush.SetResourceObject(PublicImageMap[ESLPublicWidgetImageType::EPWI_Collapsed]);
		ExpandableAreaStyle.SetCollapsedImage(SlateBrush);

		SlateBrush.SetResourceObject(PublicImageMap[ESLPublicWidgetImageType::EPWI_Expanded]);
		ExpandableAreaStyle.SetExpandedImage(SlateBrush);

		ResolutionList->SetStyle(ExpandableAreaStyle);
		LanguageList->SetStyle(ExpandableAreaStyle);
	}

	return true;
}

void USLOptionWidget::InitOptionVariable()
{
	CheckValidOfUserDataSubsystem();

	TPair<float, float> CurrentReolution =  UserDataSubsystem->GetCurrentScreenSize();

	CurrentResolutionText->SetText(FText::FromString(FString::Printf(TEXT("%.0f * %.0f"), 
		CurrentReolution.Key, CurrentReolution.Value)));

	BgmVolumeSlider->SetValue(UserDataSubsystem->GetCurrentBgmVolume());
	EffectVolumeSlider->SetValue(UserDataSubsystem->GetCurrentEffectVolume());

	BrightnessSlider->SetValue(UserDataSubsystem->GetCurrentBrightness());
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
	
	FText TargetText = FText::GetEmpty();

	if (CurrentLanguage == ESLLanguageType::EL_Kor)
	{
		TargetText = KorText->GetText();
	}
	else if (CurrentLanguage == ESLLanguageType::EL_Eng)
	{
		TargetText = EngText->GetText();
	}

	CurrentLanguageText->SetText(TargetText);
	LanguageList->SetIsExpanded(false);
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
