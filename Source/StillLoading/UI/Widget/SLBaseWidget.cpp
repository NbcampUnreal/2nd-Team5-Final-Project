// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLBaseWidget.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "Animation/WidgetAnimation.h"
#include "UI/Widget/SLWidgetImageDataAsset.h"
#include "SubSystem/SLLevelTransferSubsystem.h"

void USLBaseWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	UISubsystem = NewUISubsystem;

	if (IsValid(OpenAnim))
	{
		EndOpenAnimDelegate.BindDynamic(this, &ThisClass::OnEndedOpenAnim);
		BindToAnimationFinished(OpenAnim, EndOpenAnimDelegate);
	}

	if (IsValid(CloseAnim))
	{
		EndCloseAnimDelegate.BindDynamic(this, &ThisClass::OnEndedCloseAnim);
		BindToAnimationFinished(CloseAnim, EndCloseAnimDelegate);
	}

	CheckValidOfTextPoolSubsystem();
	TextPoolSubsystem->LanguageDelegate.AddDynamic(this, &ThisClass::NotifyChangedLanguage);

	ApplyTextData();
}

void USLBaseWidget::ApplyOnChangedChapter(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	CurrentChapter = WidgetActivateBuffer.CurrentChapter;
	FindWidgetData(WidgetActivateBuffer);

	ApplyImageData();
	ApplyFontData();
}

ESLInputModeType USLBaseWidget::GetWidgetInputMode() const
{
	return WidgetInputMode;
}

int32 USLBaseWidget::GetWidgetOrder() const
{
	return WidgetOrder;
}

bool USLBaseWidget::GetWidgetCursorMode() const
{
	return bIsVisibleCursor;
}

void USLBaseWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	if (CurrentChapter != WidgetActivateBuffer.CurrentChapter)
	{
		ApplyOnChangedChapter(WidgetActivateBuffer);
	}
}

void USLBaseWidget::OnEndedCloseAnim()
{
	this->RemoveFromViewport();
}

void USLBaseWidget::NotifyChangedLanguage()
{
	ApplyTextData();
}

void USLBaseWidget::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	const USLWidgetImageDataAsset* WidgetDataAsset = Cast<USLWidgetImageDataAsset>(WidgetActivateBuffer.WidgetPublicData);

	PublicImageMap = WidgetDataAsset->GetImageDataMap();
	FontInfo = WidgetDataAsset->GetFondInfo();
}

void USLBaseWidget::ApplyImageData()
{
	FButtonStyle ButtonStyle;
	FSliderStyle SliderStyle;
	FProgressBarStyle ProgressBarStyle;

	ApplyBackgroundImage();
	ApplyButtonImage(ButtonStyle);
	ApplySliderImage(SliderStyle);
	ApplyBorderImage();
	ApplyProgressBarImage(ProgressBarStyle);
	ApplyOtherImage();
}

bool USLBaseWidget::ApplyBackgroundImage()
{
	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_Background) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_Background]))
	{
		return false;
	}

	return true;
}

bool USLBaseWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_Button) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_Button]))
	{
		return false;
	}

	FSlateBrush SlateBrush; 
	SlateBrush.SetResourceObject(PublicImageMap[ESLPublicWidgetImageType::EPWI_Button]);

	ButtonStyle.SetNormal(SlateBrush);
	ButtonStyle.SetHovered(SlateBrush);
	ButtonStyle.SetPressed(SlateBrush);
	ButtonStyle.SetDisabled(SlateBrush);

	return true;
}

bool USLBaseWidget::ApplySliderImage(FSliderStyle& SliderStyle)
{
	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_SliderBack) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_SliderBack]))
	{
		return false;
	}

	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_SliderBar) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_SliderBar]))
	{
		return false;
	}

	FSlateBrush SlateBrush;

	SlateBrush.SetResourceObject(PublicImageMap[ESLPublicWidgetImageType::EPWI_SliderBack]);
	SliderStyle.SetNormalThumbImage(SlateBrush);
	SliderStyle.SetHoveredThumbImage(SlateBrush);
	SliderStyle.SetDisabledThumbImage(SlateBrush);

	SlateBrush.SetResourceObject(PublicImageMap[ESLPublicWidgetImageType::EPWI_SliderBar]);
	SliderStyle.SetNormalBarImage(SlateBrush);
	SliderStyle.SetHoveredBarImage(SlateBrush);
	SliderStyle.SetDisabledBarImage(SlateBrush);

	return true;
}

bool USLBaseWidget::ApplyBorderImage()
{
	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_NormalBorder) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_NormalBorder]))
	{
		return false;
	}

	return true;
}

bool USLBaseWidget::ApplyTextBorderImage()
{
	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_TextBorder) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_TextBorder]))
	{
		return false;
	}

	return true;
}

bool USLBaseWidget::ApplyProgressBarImage(FProgressBarStyle& ProgressBarStyle)
{
	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_ProgressBack) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_ProgressBack]))
	{
		return false;
	}

	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_ProgressBar) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_ProgressBar]))
	{
		return false;
	}

	FSlateBrush SlateBrush;

	SlateBrush.SetResourceObject(PublicImageMap[ESLPublicWidgetImageType::EPWI_ProgressBack]);
	ProgressBarStyle.SetBackgroundImage(SlateBrush);
	
	SlateBrush.SetResourceObject(PublicImageMap[ESLPublicWidgetImageType::EPWI_ProgressBar]);
	ProgressBarStyle.SetFillImage(SlateBrush);

	return true;
}

bool USLBaseWidget::ApplyOtherImage()
{
	return true;
}

void USLBaseWidget::PlayUISound(ESLUISoundType SoundType)
{
	CheckValidOfSoundSubsystem();
	SoundSubsystem->PlayUISound(SoundType);
}

void USLBaseWidget::MoveToLevelByType(ESLLevelNameType LevelType)
{
	USLLevelTransferSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
	checkf(IsValid(LevelSubsystem),TEXT("Level Subsystem is invalid"));

	LevelSubsystem->OpenLevelByNameType(LevelType);
}

void USLBaseWidget::CheckValidOfUISubsystem()
{
	if (IsValid(UISubsystem))
	{
		return;
	}

	checkf(IsValid(GetGameInstance()), TEXT("GameInstance is invalid"));
	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	checkf(IsValid(UISubsystem), TEXT("UISubsystem is invalid"));
}

void USLBaseWidget::CheckValidOfTextPoolSubsystem()
{
	if (IsValid(TextPoolSubsystem))
	{
		return;
	}

	TextPoolSubsystem = GetGameInstance()->GetSubsystem<USLTextPoolSubsystem>();
	checkf(IsValid(TextPoolSubsystem), TEXT("TextPool Subsystem is invalid"));
}

void USLBaseWidget::CheckValidOfSoundSubsystem()
{
	if (IsValid(SoundSubsystem))
	{
		return;
	}

	SoundSubsystem = GetGameInstance()->GetSubsystem<USLSoundSubsystem>();
	checkf(IsValid(SoundSubsystem), TEXT("Sound Subsystem is invalid"));
}
