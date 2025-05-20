// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLTitleHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"

void ASLTitleHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	CheckValidOfUISubsystem();
	UISubsystem->ActivateFade(true);
}