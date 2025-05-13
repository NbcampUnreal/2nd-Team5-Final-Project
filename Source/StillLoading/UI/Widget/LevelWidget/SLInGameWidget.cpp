// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLInGameWidget.h"

void USLInGameWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetInputMode = ESLInputModeType::EIM_GameOnly;
	bIsVisibleCursor = false;

	Super::InitWidget(NewUISubsystem, ChapterType);

}

void USLInGameWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLInGameWidget::ApplyImageData()
{
	Super::ApplyImageData();

}

void USLInGameWidget::ApplyFontData()
{
	Super::ApplyFontData();

}
