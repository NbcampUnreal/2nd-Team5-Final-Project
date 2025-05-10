// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLFadeWidget.h"
#include "Components/Image.h"

void USLFadeWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::FadeWidget;

	OpenAnim = FadeInAnim;
	CloseAnim = FadeOutAnim;

	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLFadeWidget::ActivateWidget(ESLChapterType ChapterType)
{
	if (CurrentChapter != ChapterType)
	{
		ApplyOnChangedChapter(ChapterType);
	}

	if (bIsFadeIn)
	{
		PlayAnimation(OpenAnim);
	}
	else
	{
		PlayAnimation(CloseAnim);
	}
}

void USLFadeWidget::DeactivateWidget()
{
	bIsFadeIn = !bIsFadeIn;
}

void USLFadeWidget::OnEndedOpenAnim()
{
	CloseWidget();
}

void USLFadeWidget::OnEndedCloseAnim()
{
	CloseWidget();
}

void USLFadeWidget::ApplyImageData()
{
	//FadeImage->SetBrushFromTexture();
}
