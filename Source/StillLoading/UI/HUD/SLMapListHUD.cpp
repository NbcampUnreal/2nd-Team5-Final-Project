// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLMapListHUD.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"

void ASLMapListHUD::OnStartedHUD()
{
	Super::OnStartedHUD();

	CheckValidOfUISubsystem();
	UISubsystem->ActivateFade(true);
}