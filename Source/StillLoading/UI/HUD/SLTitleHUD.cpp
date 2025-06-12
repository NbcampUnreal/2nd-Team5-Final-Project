// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLTitleHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"

void ASLTitleHUD::NotifyTalkEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("Recieve End Talk"));
}

void ASLTitleHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	CheckValidOfUISubsystem();
	UISubsystem->ActivateFade(true);

	//UISubsystem->ActivateTalk(ESLTalkTargetType::ETT_Heroine, "Serena", "TitleTalk").OnTalkEnded.AddDynamic(this, &ThisClass::NotifyTalkEnded);
}