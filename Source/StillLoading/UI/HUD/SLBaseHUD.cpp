// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SLBaseHUD.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "UI/SLUISubsystem.h"

void ASLBaseHUD::SetChapterToLevelWidget(ESLChapterType ChapterType)
{
	CheckValidOfUISubsystem();
	UISubsystem->SetChapterToUI(ChapterType);

	CheckValidOfLevelWidget();
	checkf(ChapterWidgetDataMap.Contains(ChapterType), TEXT("Widget Data Map is not contains ChapterType"));

	LevelWidgetObj->SetLevelWidgetData(ChapterWidgetDataMap[ChapterType]);
	LevelWidgetObj->ActivateWidget(ChapterType);
	LevelWidgetObj->AddToViewport(0);

	ApplyLevelWidgetInputMode();
}

void ASLBaseHUD::SetLanguageToLevelWidget(ESLLanguageType LanguageType)
{
	CheckValidOfLevelWidget();
	LevelWidgetObj->ApplyOnChangedLanguage(LanguageType);
}

void ASLBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	OnStartedHUD();
}

void ASLBaseHUD::ApplyLevelWidgetInputMode()
{
	APlayerController* OwningPC = GetOwningPlayerController();
	checkf(IsValid(OwningPC), TEXT("PlayerController is invalid"));

	CheckValidOfLevelWidget();

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

	InitLevelWidget();
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
