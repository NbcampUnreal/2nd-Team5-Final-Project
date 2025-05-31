// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLGraphicSettingWidget.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ExpandableArea.h"
#include "Components/Slider.h"

const TArray<TPair<float, float>> USLGraphicSettingWidget::ResolutionSet = { {1920, 1080}, {1280, 800}, {1680, 1050} };

const FName USLGraphicSettingWidget::WindowModeTagIndex = "WindowModeTag";
const FName USLGraphicSettingWidget::ResolutionTagIndex = "ResolutionTag";
const FName USLGraphicSettingWidget::BrigthnessTagIndex = "BrigthnessTag";
const FName USLGraphicSettingWidget::FullScreenButtonIndex = "FullScreenButton";
const FName USLGraphicSettingWidget::WindowedButtonIndex = "WindowedButton";


void USLGraphicSettingWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	Super::InitWidget(NewUISubsystem);

	FullScreenButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedFullScreen);
	WindowScreenButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedWindowScreen);

	FirstResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedFirstResolution);
	SecondResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedSecondResolution);
	ThirdResolutionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedThirdResolution);

	BrightnessSlider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateBrightness);

	CheckValidOfUserDataSubsystem();

	TPair<float, float> CurrentReolution = UserDataSubsystem->GetCurrentScreenSize();

	CurrentResolutionText->SetText(FText::FromString(FString::Printf(TEXT("%.0f * %.0f"),
		CurrentReolution.Key, CurrentReolution.Value)));

	BrightnessSlider->SetValue(UserDataSubsystem->GetCurrentBrightness());
}

void USLGraphicSettingWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	UpdateScreenModeButton();
}

void USLGraphicSettingWidget::ApplyTextData()
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

	WindowModeText->SetText(OptionTextMap[WindowModeTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	FullScreenText->SetText(OptionTextMap[FullScreenButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	WindowScreenText->SetText(OptionTextMap[WindowedButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	ResolutionText->SetText(OptionTextMap[ResolutionTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	BrightnessText->SetText(OptionTextMap[BrigthnessTagIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
}

bool USLGraphicSettingWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyButtonImage(ButtonStyle))
	{
		return false;
	}

	FullScreenButton->SetStyle(ButtonStyle);
	WindowScreenButton->SetStyle(ButtonStyle);

	return true;
}

bool USLGraphicSettingWidget::ApplySliderImage(FSliderStyle& SliderStyle)
{
	if (!Super::ApplySliderImage(SliderStyle))
	{
		return false;
	}

	BrightnessSlider->SetWidgetStyle(SliderStyle);

	return true;
}

bool USLGraphicSettingWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	ApplyExpandableImage();
	ApplyExpandedImage();

	return true;
}

void USLGraphicSettingWidget::OnClickedFullScreen()
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetWindowMode(EWindowMode::Fullscreen);
	UpdateScreenModeButton();
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLGraphicSettingWidget::OnClickedWindowScreen()
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetWindowMode(EWindowMode::Windowed);
	UpdateScreenModeButton();
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLGraphicSettingWidget::OnClickedFirstResolution()
{
	UpdateResolution(0);
}

void USLGraphicSettingWidget::OnClickedSecondResolution()
{
	UpdateResolution(1);
}

void USLGraphicSettingWidget::OnClickedThirdResolution()
{
	UpdateResolution(2);
}

void USLGraphicSettingWidget::UpdateBrightness(float BrightnessValue)
{
	CheckValidOfUserDataSubsystem();
	UserDataSubsystem->SetBrightness(BrightnessValue);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLGraphicSettingWidget::UpdateScreenModeButton()
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

void USLGraphicSettingWidget::UpdateResolution(int32 ResolutionNum)
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

void USLGraphicSettingWidget::ApplyExpandableImage()
{
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_Expandable) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Expandable]))
	{
		FSlateBrush SlateBrush;

		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Expandable]);
		ResolutionList->SetBorderBrush(SlateBrush);
	}
}

void USLGraphicSettingWidget::ApplyExpandedImage()
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

void USLGraphicSettingWidget::CheckValidOfUserDataSubsystem()
{
	if (IsValid(UserDataSubsystem))
	{
		return;
	}

	UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();
	checkf(IsValid(UserDataSubsystem), TEXT("User Data Subsystem is invalid"));
}
