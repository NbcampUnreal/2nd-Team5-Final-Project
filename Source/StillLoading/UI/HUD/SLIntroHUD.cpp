// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLIntroHUD.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"

void ASLIntroHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	ApplyLevelWidgetInputMode();
	LevelWidgetObj->ActivateWidget(CurrentChapter);
	LevelWidgetObj->AddToViewport(0);
}

void ASLIntroHUD::InitLevelWidget()
{
	Super::InitLevelWidget();

	CheckValidOfUISubsystem();
	LevelWidgetObj->InitWidget(UISubsystem, CurrentChapter);
}
