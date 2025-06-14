// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLOptionSubBase.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/SLWidgetPrivateDataAsset.h"

void USLOptionSubBase::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::FindWidgetData(WidgetActivateBuffer);

	if (IsValid(WidgetActivateBuffer.WidgetPrivateData))
	{
		USLOptionPrivateDataAsset* PrivateData = Cast<USLOptionPrivateDataAsset>(WidgetActivateBuffer.WidgetPrivateData);
		PrivateImageMap.Empty();
		PrivateImageMap = PrivateData->GetBrushDataMap();
	}
}

bool USLOptionSubBase::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	FSliderStyle SliderStyle;
	FButtonStyle ButtonStyle;
	FSlateBrush SlateBrush;

	ApplySliderImage(SliderStyle);
	ApplyExpandableImage(SlateBrush);
	ApplyExpandedImage(ButtonStyle);
	ApplyListBackImage(SlateBrush);
	ApplyLeftArrowImage(ButtonStyle);
	ApplyRightArrowImage(ButtonStyle);

	return true;
}

bool USLOptionSubBase::ApplySliderImage(FSliderStyle& SliderStyle)
{
	if (!PrivateImageMap.Contains(ESLOptionPrivateImageType::EOPI_SliderBack) ||
		!IsValid(PrivateImageMap[ESLOptionPrivateImageType::EOPI_SliderBack]))
	{
		return false;
	}

	if (!PrivateImageMap.Contains(ESLOptionPrivateImageType::EOPI_SliderBar) ||
		!IsValid(PrivateImageMap[ESLOptionPrivateImageType::EOPI_SliderBar]))
	{
		return false;
	}

	FSlateBrush SlateBrush;

	SlateBrush.SetResourceObject(PrivateImageMap[ESLOptionPrivateImageType::EOPI_SliderBar]);
	SliderStyle.SetNormalThumbImage(SlateBrush);
	SliderStyle.SetHoveredThumbImage(SlateBrush);
	SliderStyle.SetDisabledThumbImage(SlateBrush);

	SlateBrush.SetResourceObject(PrivateImageMap[ESLOptionPrivateImageType::EOPI_SliderBack]);
	SliderStyle.SetNormalBarImage(SlateBrush);
	SliderStyle.SetHoveredBarImage(SlateBrush);
	SliderStyle.SetDisabledBarImage(SlateBrush);

	return true;
}

bool USLOptionSubBase::ApplyExpandableImage(FSlateBrush& SlateBrush)
{
	if (PrivateImageMap.Contains(ESLOptionPrivateImageType::EOPI_Expandable) &&
		IsValid(PrivateImageMap[ESLOptionPrivateImageType::EOPI_Expandable]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLOptionPrivateImageType::EOPI_Expandable]);
		return true;
	}

	return false;
}

bool USLOptionSubBase::ApplyOptionButtonImage(FButtonStyle& ButtonStyle, ESLOptionPrivateImageType ButtonType)
{
	if (PrivateImageMap.Contains(ButtonType) &&
		IsValid(PrivateImageMap[ButtonType]))
	{
		FSlateBrush SlateBrush;

		SlateBrush.SetResourceObject(PrivateImageMap[ButtonType]);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.75f, 0.75f, 0.75f, 1.0f));
		ButtonStyle.SetNormal(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		ButtonStyle.SetHovered(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
		ButtonStyle.SetPressed(SlateBrush);

		SlateBrush.TintColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));
		ButtonStyle.SetDisabled(SlateBrush);

		return true;
	}

	return false;
}

bool USLOptionSubBase::ApplyExpandedImage(FButtonStyle& ButtonStyle)
{
	if (ApplyOptionButtonImage(ButtonStyle, ESLOptionPrivateImageType::EOPI_ExpandedButton))
	{
		return true;
	}

	return false;
}

bool USLOptionSubBase::ApplyListBackImage(FSlateBrush& SlateBrush)
{
	if (PrivateImageMap.Contains(ESLOptionPrivateImageType::EOPI_ListBack) &&
		IsValid(PrivateImageMap[ESLOptionPrivateImageType::EOPI_ListBack]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLOptionPrivateImageType::EOPI_ListBack]);
		return true;
	}

	return false;
}

bool USLOptionSubBase::ApplyLeftArrowImage(FButtonStyle& ButtonStyle)
{
	if (ApplyOptionButtonImage(ButtonStyle, ESLOptionPrivateImageType::EOPI_LeftArrow))
	{
		return true;
	}

	return false;
}

bool USLOptionSubBase::ApplyRightArrowImage(FButtonStyle& ButtonStyle)
{
	if (ApplyOptionButtonImage(ButtonStyle, ESLOptionPrivateImageType::EOPI_RightArrow))
	{
		return true;
	}

	return false;
}
