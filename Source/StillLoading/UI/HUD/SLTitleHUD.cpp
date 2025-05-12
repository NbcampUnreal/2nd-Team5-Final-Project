// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLTitleHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"

void ASLTitleHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	CheckValidOfUISubsystem();
	UISubsystem->AddAdditveWidget(ESLAdditiveWidgetType::EAW_FadeWidget);
}

void ASLTitleHUD::InitLevelWidget()
{
	Super::InitLevelWidget();

	checkf(ChapterWidgetDataMap.Contains(CurrentChapter), TEXT("Widget Data Map is not contains ChapterType"));
	LevelWidgetObj->SetLevelWidgetData(ChapterWidgetDataMap[CurrentChapter]);

	CheckValidOfUISubsystem();
	LevelWidgetObj->InitWidget(UISubsystem, CurrentChapter);
}
