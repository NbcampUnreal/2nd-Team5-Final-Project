// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLTitleHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLTitleWidget.h"

void ASLTitleHUD::MoveToStartMap()
{
	CheckValidOfLevelWidget();
	
	USLTitleWidget* TitleWidget = Cast<USLTitleWidget>(LevelWidgetObj);

	if (IsValid(TitleWidget))
	{
		TitleWidget->OnClickedContinueButton();
	}
}

void ASLTitleHUD::NotifyTalkEnded()
{
	LevelWidgetObj->AddToViewport();
}

void ASLTitleHUD::ActivateTitleTalk()
{
	CheckValidOfUISubsystem();
	UISubsystem->GetTalkWidget()->OnTalkEnded.AddDynamic(this, &ThisClass::NotifyTalkEnded);
	UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_Heroine, "Serena", "TitleTalk");
}

void ASLTitleHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	LevelWidgetObj->RemoveFromViewport();

	CheckValidOfUISubsystem();

	UISubsystem->ActivateFade(true);
}