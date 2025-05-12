// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLMapListWidget.h"

void USLMapListWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;

	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLMapListWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLMapListWidget::ApplyImageData()
{
	Super::ApplyImageData();
}

void USLMapListWidget::ApplyFontData()
{
	Super::ApplyFontData();
}
