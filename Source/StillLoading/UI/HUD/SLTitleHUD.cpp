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

void ASLTitleHUD::NotifyChoiceEnded(bool bIsAccept)
{
	if (bIsAccept)
	{
		UE_LOG(LogTemp, Warning, TEXT("Plyaer Choice Aceept"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Plyaer Choice Reject"));
	}

	LevelWidgetObj->AddToViewport();
}

void ASLTitleHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	LevelWidgetObj->RemoveFromViewport();

	CheckValidOfUISubsystem();

	UISubsystem->ActivateFade(true);
	UISubsystem->GetTalkWidget()->OnChoiceEnded.AddDynamic(this, &ThisClass::NotifyChoiceEnded);
	UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_Heroine, "Serena", "TitleTalk");
}