// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLOptionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ExpandableArea.h"
#include "Components/Slider.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLKeySettingWidget.h"
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

	LanguageLeftButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedLanguageLeftButton);
	LanguageRightButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedLanguageRightButton);

	FirstResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedFirstResolution);
	SecondResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedSecondResolution);
	ThirdResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedThirdResolution);

	BgmVolumeSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateBgmVolume);
	EffectVolumeSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateEffectVolume);

	BrightnessSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateBrightness);

	LanguageSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedLanguageSetting);
	GraphicSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedGraphicSetting);
	SoundSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedSoundSetting);
	KeySettingButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedKeySetting);
	QuitGameButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuit);
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);

	InitOptionVariable();
	KeySettingWidget->InitWidget(NewUISubsystem);
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

	LayerSwitcher->SetActiveWidgetIndex(0);
	KeySettingWidget->ActivateWidget(WidgetActivateBuffer);

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

bool USLOptionWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyButtonImage(ButtonStyle))
	{
		return false;
	}

	FullScreenButton->SetStyle(ButtonStyle);
	WindowScreenButton->SetStyle(ButtonStyle);
	KeySettingButton->SetStyle(ButtonStyle);
	QuitGameButton->SetStyle(ButtonStyle);
	CloseButton->SetStyle(ButtonStyle);
	LanguageSetBt->SetStyle(ButtonStyle);
	GraphicSetBt->SetStyle(ButtonStyle);
	SoundSetBt->SetStyle(ButtonStyle);

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

bool USLOptionWidget::ApplyBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyBorderImage(SlateBrush))
	{
		return false;
	}

	OptionPanelBack->SetBrush(SlateBrush);

	return true;
}

bool USLOptionWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	ApplyExpandableImage();
	ApplyExpandedImage();
	ApplyListBackImage();
	ApplyLeftArrowImage();
	ApplyRightArrowImage();

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

void USLOptionWidget::OnClickedLanguageSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(0);
}

void USLOptionWidget::OnClickedGraphicSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(1);
}

void USLOptionWidget::OnClickedSoundSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(2);
}

void USLOptionWidget::OnClickedKeySetting()
{
	LayerSwitcher->SetActiveWidgetIndex(3);
	/*CheckValidOfUISubsystem();
	UISubsystem->AddAdditiveWidget(ESLAdditiveWidgetType::EAW_KeySettingWidget);*/
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

void USLOptionWidget::OnClickedLanguageLeftButton()
{
	UpdateLanguage(true);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedLanguageRightButton()
{
	UpdateLanguage(false);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::UpdateLanguage(bool bIsLeft)
{
	CheckValidOfUserDataSubsystem();
	ESLLanguageType CurrentLanguage = UserDataSubsystem->GetCurrentLanguage();
	int32 NextLanguageIndex = 0;

	if (bIsLeft)
	{
		NextLanguageIndex = (int32)CurrentLanguage - 1;
	}
	else
	{
		NextLanguageIndex = (int32)CurrentLanguage + 1;
	}

	if (NextLanguageIndex == (int32)ESLLanguageType::EL_None)
	{
		NextLanguageIndex = (int32)ESLLanguageType::EL_Max - 1;
	}
	else if (NextLanguageIndex == (int32)ESLLanguageType::EL_Max)
	{
		NextLanguageIndex = (int32)ESLLanguageType::EL_None + 1;
	}

	CurrentLanguage = (ESLLanguageType)NextLanguageIndex;

	if (LanguageTextMap.Contains(CurrentLanguage))
	{
		UserDataSubsystem->SetLanguage(CurrentLanguage);
		LanguageText->SetText(LanguageTextMap[CurrentLanguage]);
	}
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
	
	if (LanguageTextMap.Contains(CurrentLanguage))
	{
		LanguageText->SetText(LanguageTextMap[CurrentLanguage]);
	}
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

void USLOptionWidget::ApplyExpandableImage()
{
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_Expandable) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Expandable]))
	{
		FSlateBrush SlateBrush;

		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Expandable]);
		ResolutionList->SetBorderBrush(SlateBrush);
	}
}

void USLOptionWidget::ApplyExpandedImage()
{
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ExpandedButton) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ExpandedButton]))
	{
		FButtonStyle ButtonStyle;
		FSlateBrush SlateBrush;

		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ExpandedButton]);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.75f, 0.75f, 0.75f, 1.0f));
		ButtonStyle.SetNormal(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		ButtonStyle.SetHovered(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
		ButtonStyle.SetPressed(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));
		ButtonStyle.SetDisabled(SlateBrush);

		FirstResolutionButton->SetStyle(ButtonStyle);
		SecondResolutionButton->SetStyle(ButtonStyle);
		ThirdResolutionButton->SetStyle(ButtonStyle);
	}
}

void USLOptionWidget::ApplyListBackImage()
{
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ListBack) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ListBack]))
	{
		FSlateBrush SlateBrush;
		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ListBack]);
		LanguageBack->SetBrush(SlateBrush);
	}
}

void USLOptionWidget::ApplyLeftArrowImage()
{
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_LeftArrow) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_LeftArrow]))
	{
		FButtonStyle ButtonStyle;
		FSlateBrush SlateBrush;

		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_LeftArrow]);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.75f, 0.75f, 0.75f, 1.0f));
		ButtonStyle.SetNormal(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		ButtonStyle.SetHovered(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
		ButtonStyle.SetPressed(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));
		ButtonStyle.SetDisabled(SlateBrush);

		LanguageLeftButton->SetStyle(ButtonStyle);
	}
}

void USLOptionWidget::ApplyRightArrowImage()
{
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_RightArrow) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_RightArrow]))
	{
		FButtonStyle ButtonStyle;
		FSlateBrush SlateBrush;

		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_RightArrow]);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.75f, 0.75f, 0.75f, 1.0f));
		ButtonStyle.SetNormal(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		ButtonStyle.SetHovered(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
		ButtonStyle.SetPressed(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));
		ButtonStyle.SetDisabled(SlateBrush);

		LanguageRightButton->SetStyle(ButtonStyle);
	}
}
