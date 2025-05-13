// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLFadeWidget.h"
#include "Components/Image.h"

const FName USLFadeWidget::FadeImgName = "FadeImage";

void USLFadeWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::EAW_FadeWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = false;

	OpenAnim = FadeInAnim;
	CloseAnim = FadeOutAnim;

	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLFadeWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	if (bIsFadeIn)
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

void USLFadeWidget::SetIsFadeIn(bool FadeValue)
{
	bIsFadeIn = FadeValue;
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

	checkf(ImageMap.Contains(FadeImgName), TEXT("Image Name is Not Contains"));
	checkf(IsValid(ImageMap[FadeImgName]), TEXT("Image Source is invalid"));
	FadeImage->SetBrushFromTexture(ImageMap[FadeImgName]);
}
