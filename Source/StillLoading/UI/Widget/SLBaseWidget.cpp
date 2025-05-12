// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLBaseWidget.h"
#include "UI/SLUISubsystem.h"
#include "Animation/WidgetAnimation.h"

void USLBaseWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
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

	ApplyOnChangedChapter(ChapterType);
}

void USLBaseWidget::ApplyOnChangedChapter(ESLChapterType ChapterType)
{
	CurrentChapter = ChapterType;
	ImageMap.Empty();

	FindWidgetData();

	ApplyImageData();
	ApplyFontData();
}

void USLBaseWidget::ActivateWidget(ESLChapterType ChapterType)
{
	if (CurrentChapter != ChapterType)
	{
		ApplyOnChangedChapter(ChapterType);
	}
}

void USLBaseWidget::OnEndedCloseAnim()
{
	this->RemoveFromViewport();
}

void USLBaseWidget::PlayUISound(ESLUISoundType SoundType)
{
	CheckValidOfUISubsystem();
	UISubsystem->PlayUISound(SoundType);
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
