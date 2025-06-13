// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLTitleHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"

void ASLTitleHUD::NotifyTalkEnded()
{
	LevelWidgetObj->AddToViewport();
}

void ASLTitleHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	LevelWidgetObj->RemoveFromViewport();

	CheckValidOfUISubsystem();

	UISubsystem->ActivateFade(true);
	UISubsystem->GetTalkWidget()->OnTalkEnded.AddDynamic(this, &ThisClass::NotifyTalkEnded);
	UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_Heroine, "Serena", "TitleTalk");
}