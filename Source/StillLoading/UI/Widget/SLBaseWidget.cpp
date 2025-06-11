// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLBaseWidget.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "Animation/WidgetAnimation.h"
#include "UI/Widget/SLWidgetImageDataAsset.h"
#include "UI/Widget/SLButtonWidget.h"
#include "SubSystem/SLLevelTransferSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "NiagaraSystem.h"

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
	CheckValidOfTextPoolSubsystem();
	if (!TextPoolSubsystem->LanguageDelegate.IsAlreadyBound(this, &ThisClass::NotifyChangedLanguage))
	{
		TextPoolSubsystem->LanguageDelegate.AddDynamic(this, &ThisClass::NotifyChangedLanguage);
	}

	if (CurrentChapter != WidgetActivateBuffer.CurrentChapter)
	{
		ApplyOnChangedChapter(WidgetActivateBuffer);
	}
}

void USLBaseWidget::DeactivateWidget()
{
	CheckValidOfTextPoolSubsystem();
	if (TextPoolSubsystem->LanguageDelegate.IsAlreadyBound(this, &ThisClass::NotifyChangedLanguage))
	{
		TextPoolSubsystem->LanguageDelegate.RemoveDynamic(this, &ThisClass::NotifyChangedLanguage);
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

void USLBaseWidget::PlayHoverSound()
{
	PlayUISound(ESLUISoundType::EUS_Hover);
}

void USLBaseWidget::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	const USLWidgetImageDataAsset* WidgetDataAsset = Cast<USLWidgetImageDataAsset>(WidgetActivateBuffer.WidgetPublicData);

	PublicAssetMap = WidgetDataAsset->GetBrushDataMap();
	FontInfo = WidgetDataAsset->GetFondInfo();
	FontOffset = WidgetDataAsset->GetFontOffset();
}

void USLBaseWidget::ApplyImageData()
{
	FButtonStyle ButtonStyle;
	FSliderStyle SliderStyle;
	FProgressBarStyle ProgressBarStyle;
	FSlateBrush SlateBrush;

	ApplyBackgroundImage(SlateBrush);
	ApplyButtonImage(ButtonStyle);
	ApplySliderImage(SliderStyle);
	ApplyBorderImage(SlateBrush);
	ApplyTextBorderImage(SlateBrush);
	ApplyProgressBarImage(ProgressBarStyle);
	ApplyOtherImage();
}

void USLBaseWidget::ApplyFontData()
{
	TArray<UWidget*> FoundWidgets;
	WidgetTree->GetAllWidgets(FoundWidgets);

	for (UWidget* Widget : FoundWidgets)
	{
		if (IsValid(Widget))
		{
			UTextBlock* TextBlock = Cast<UTextBlock>(Widget);

			if (IsValid(TextBlock))
			{
				FontInfo.Size = TextBlock->GetFont().Size;
				TextBlock->SetFont(FontInfo);
				TextBlock->SetRenderTranslation(FVector2D(0, FontOffset));
			}

			USLButtonWidget* CustomButton = Cast<USLButtonWidget>(Widget);

			if (IsValid(CustomButton))
			{
				CustomButton->SetButtonFont(FontInfo, FontOffset);
			}
		}
	}
}

bool USLBaseWidget::ApplyBackgroundImage(FSlateBrush& SlateBrush)
{
	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_Background) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Background]))
	{
		return false;
	}

	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Background]);

	return true;
}

bool USLBaseWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	bool bIsContainNormal = ApplyButtonNormal(ButtonStyle);
	ApplyButtonHover(ButtonStyle, bIsContainNormal);

	TArray<UWidget*> FoundWidgets;
	WidgetTree->GetAllWidgets(FoundWidgets);

	for (UWidget* Widget : FoundWidgets)
	{
		if (IsValid(Widget))
		{
			USLButtonWidget* CustomButton = Cast<USLButtonWidget>(Widget);

			if (IsValid(CustomButton))
			{
				CustomButton->SetButtonStyle(ButtonStyle);
				ApplyButtonRetainer(CustomButton);
				ApplyButtonNiagara(CustomButton);
			}
		}
	}

	return true;
}

bool USLBaseWidget::ApplyButtonNormal(FButtonStyle& ButtonStyle)
{
	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_Button) || 
		(PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_Button) &&
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Button])))
	{
		return false;
	}

	FSlateBrush NormalBrush;
	FSlateBrush HoverBrush;
	FSlateBrush PressedBrush;
	FSlateBrush DisableedBrush;

	UObject* Resource = PublicAssetMap[ESLPublicWidgetImageType::EPWI_Button];

	NormalBrush.SetResourceObject(Resource);
	HoverBrush.SetResourceObject(Resource);
	PressedBrush.SetResourceObject(Resource);
	DisableedBrush.SetResourceObject(Resource);

	NormalBrush.TintColor = FSlateColor(FLinearColor(0.75f, 0.75f, 0.75f, 0.75f));
	HoverBrush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	PressedBrush.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
	DisableedBrush.TintColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));

	ButtonStyle.SetNormal(NormalBrush);
	ButtonStyle.SetHovered(HoverBrush);
	ButtonStyle.SetPressed(PressedBrush);
	ButtonStyle.SetDisabled(DisableedBrush);

	return true;
}

void USLBaseWidget::ApplyButtonHover(FButtonStyle& ButtonStyle, bool bIsActiveNormal)
{
	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ButtonHover) ||
		(PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ButtonHover) &&
			!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonHover])))
	{
		return;
	}

	FSlateBrush NormalBrush;
	FSlateBrush HoverBrush;
	FSlateBrush PressedBrush;
	FSlateBrush DisableedBrush;

	UObject* Resource = PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonHover];

	NormalBrush = ButtonStyle.Normal;
	HoverBrush.SetResourceObject(Resource);
	PressedBrush.SetResourceObject(Resource);
	DisableedBrush = ButtonStyle.Disabled;

	if (bIsActiveNormal)
	{
		NormalBrush.TintColor = FSlateColor(FLinearColor(0.75f, 0.75f, 0.75f, 0.75f));
		DisableedBrush.TintColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));
	}
	else
	{
		NormalBrush.TintColor = FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
		DisableedBrush.TintColor = FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
	}

	HoverBrush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	PressedBrush.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));

	ButtonStyle.SetNormal(NormalBrush);
	ButtonStyle.SetHovered(HoverBrush);
	ButtonStyle.SetPressed(PressedBrush);
	ButtonStyle.SetDisabled(DisableedBrush);
}

void USLBaseWidget::ApplyButtonRetainer(USLButtonWidget* ButtonWidget)
{
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ButtonEffect) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonEffect]))
	{
		UNiagaraSystem* Niagara = Cast<UNiagaraSystem>(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonEffect]);

		if (IsValid(Niagara))
		{
			ButtonWidget->SetNiagaraWidget(Niagara);
		}
	}
}

void USLBaseWidget::ApplyButtonNiagara(USLButtonWidget* ButtonWidget)
{
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ButtonRetainer) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonRetainer]))
	{
		UMaterialInterface* EffectMat = Cast<UMaterialInterface>(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonRetainer]);

		if (IsValid(EffectMat))
		{
			ButtonWidget->SetRetainerMat(EffectMat);
		}
	}
}

bool USLBaseWidget::ApplySliderImage(FSliderStyle& SliderStyle)
{
	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_SliderBack) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_SliderBack]))
	{
		return false;
	}

	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_SliderBar) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_SliderBar]))
	{
		return false;
	}

	FSlateBrush SlateBrush;

	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_SliderBar]);
	SliderStyle.SetNormalThumbImage(SlateBrush);
	SliderStyle.SetHoveredThumbImage(SlateBrush);
	SliderStyle.SetDisabledThumbImage(SlateBrush);

	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_SliderBack]);
	SliderStyle.SetNormalBarImage(SlateBrush);
	SliderStyle.SetHoveredBarImage(SlateBrush);
	SliderStyle.SetDisabledBarImage(SlateBrush);

	return true;
}

bool USLBaseWidget::ApplyBorderImage(FSlateBrush& SlateBrush)
{
	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_NormalBorder) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_NormalBorder]))
	{
		return false;
	}

	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_NormalBorder]);

	return true;
}

bool USLBaseWidget::ApplyTextBorderImage(FSlateBrush& SlateBrush)
{
	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_TextBorder) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_TextBorder]))
	{
		return false;
	}

	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_TextBorder]);

	return true;
}

bool USLBaseWidget::ApplyProgressBarImage(FProgressBarStyle& ProgressBarStyle)
{
	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ProgressBack) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ProgressBack]))
	{
		return false;
	}

	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ProgressBar) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ProgressBar]))
	{
		return false;
	}

	FSlateBrush SlateBrush;

	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ProgressBack]);
	ProgressBarStyle.SetBackgroundImage(SlateBrush);
	
	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ProgressBar]);
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

void USLBaseWidget::MoveToLevelByType(ESLLevelNameType LevelType, bool bIsFade)
{
	USLLevelTransferSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
	checkf(IsValid(LevelSubsystem),TEXT("Level Subsystem is invalid"));

	LevelSubsystem->OpenLevelByNameType(LevelType, bIsFade);
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
