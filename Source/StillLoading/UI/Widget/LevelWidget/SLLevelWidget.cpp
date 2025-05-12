// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "UI/SLUISubsystem.h"

void USLLevelWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	CheckValidOfUISubsystem();
	UISubsystem->SetLevelInputMode(WidgetInputMode, bIsVisibleCursor);
}

void USLLevelWidget::FindWidgetData()
{
	Super::FindWidgetData();

	// TODO : Get Data From HUD
}

void USLLevelWidget::RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType TargetWidgetType)
{
	CheckValidOfUISubsystem();
	UISubsystem->AddAdditveWidget(TargetWidgetType);
}
