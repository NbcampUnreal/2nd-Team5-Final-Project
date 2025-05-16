// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "UI/SLUISubsystem.h"
#include "UI/Struct/SLLevelWidgetDataRow.h"

void USLLevelWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetOrder = 0;
	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLLevelWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	CheckValidOfUISubsystem();
	UISubsystem->SetLevelInputMode(WidgetInputMode, bIsVisibleCursor);
}

void USLLevelWidget::SetLevelWidgetData(const FSLLevelWidgetDataRow& LevelWidgetData)
{
	FontInfo = LevelWidgetData.FontInfo;
	ImageMap.Empty();

	for (const TPair<FName, TSoftObjectPtr<UTexture2D>>& ImagePair : LevelWidgetData.ImageMap)
	{
		ImageMap.Add(ImagePair.Key, ImagePair.Value.LoadSynchronous());
	}
}

void USLLevelWidget::RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType TargetWidgetType)
{
	CheckValidOfUISubsystem();
	UISubsystem->AddAdditveWidget(TargetWidgetType);
}
