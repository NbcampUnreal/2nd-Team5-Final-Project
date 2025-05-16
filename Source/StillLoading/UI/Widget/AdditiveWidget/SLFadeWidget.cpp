// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLFadeWidget.h"
#include "Components/Image.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"

const FName USLFadeWidget::FadeImgName = "FadeImage";

void USLFadeWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_FadeWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 20;
	bIsVisibleCursor = false;

	OpenAnim = FadeInAnim;
	CloseAnim = FadeOutAnim;

	Super::InitWidget(NewUISubsystem);
}

void USLFadeWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	if (WidgetActivateBuffer.bIsFade)
	{
		if (IsPlayingAnimation())
		{
			StopAllAnimations();
		}
		PlayAnimation(OpenAnim);
	}
	else
	{
		if (IsPlayingAnimation())
		{
			StopAllAnimations();
		}
		PlayAnimation(CloseAnim);
	}
}

void USLFadeWidget::OnEndedOpenAnim()
{
	Super::OnEndedOpenAnim();

	OnEndedCloseAnim();
}

void USLFadeWidget::OnEndedCloseAnim()
{
	Super::OnEndedCloseAnim();

	CloseWidget();
}

void USLFadeWidget::ApplyImageData()
{
	Super::ApplyImageData();

	if (ImageMap.Contains(FadeImgName) &&
		IsValid(ImageMap[FadeImgName]))
	{
		FadeImage->SetBrushFromTexture(ImageMap[FadeImgName]);
	}
	
}
