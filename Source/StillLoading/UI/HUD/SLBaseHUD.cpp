// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLBaseHUD.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "UI/SLUISubsystem.h"


void ASLBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	OnStartedHUD();
}

void ASLBaseHUD::OnStartedHUD()
{
	CheckValidOfUISubsystem();
	CurrentChapter = UISubsystem->GetCurrentChapter();

	CheckValidOfLevelWidget();
	LevelWidgetObj->InitWidget(UISubsystem);
	ApplyLevelWidgetInputMode();

	FSLWidgetActivateBuffer ActivateBuffer;
	ActivateBuffer.CurrentChapter = CurrentChapter;

	if (!IsValid(ActivateBuffer.LevelWidgetData))
	{
		ActivateBuffer.LevelWidgetData = LevelWidgetData.LoadSynchronous();
	}
	
	if (!IsValid(ActivateBuffer.WidgetImageData))
	{
		ActivateBuffer.WidgetImageData = WidgetImageData.LoadSynchronous();
	}

	LevelWidgetObj->ActivateWidget(ActivateBuffer);
	LevelWidgetObj->AddToViewport(0);
}

void ASLBaseHUD::ApplyLevelWidgetInputMode()
{
	APlayerController* OwningPC = GetOwningPlayerController();
	checkf(IsValid(OwningPC), TEXT("PlayerController is invalid"));

	ESLInputModeType InputMode = LevelWidgetObj->GetWidgetInputMode();

	switch (InputMode)
	{
	case ESLInputModeType::EIM_UIOnly:
		OwningPC->SetInputMode(FInputModeUIOnly());
		break;

	case ESLInputModeType::EIM_GameOnly:
		OwningPC->SetInputMode(FInputModeGameOnly());
		break;

	case ESLInputModeType::EIM_GameAndUI:
		OwningPC->SetInputMode(FInputModeGameAndUI());
		break;

	default:
		checkNoEntry();
	}

	OwningPC->SetShowMouseCursor(LevelWidgetObj->GetWidgetCursorMode());
}

void ASLBaseHUD::CheckValidOfLevelWidget()
{
	if (IsValid(LevelWidgetObj))
	{
		return;
	}

	checkf(IsValid(LevelWidgetClass), TEXT("LevelWidgetClass is invalid"));
	LevelWidgetObj = CreateWidget<USLLevelWidget>(GetOwningPlayerController(), LevelWidgetClass);
	checkf(IsValid(LevelWidgetObj), TEXT("LevelWidgetObj is invalid"));
}

void ASLBaseHUD::CheckValidOfUISubsystem()
{
	if (IsValid(UISubsystem))
	{
		return;
	}

	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();
	checkf(IsValid(UISubsystem), TEXT("UISubsystem is invalid"));
}
