// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SLUISubsystem.h"
#include "UI/SLUISettings.h"
#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "UI/Widget/AdditiveWidget/SLStoryWidget.h"
#include "UI/HUD/SLInGameHUD.h"

void USLUISubsystem::SetInputModeAndCursor(bool bIsRemove)
{
	ESLInputModeType TargetInputMode = ESLInputModeType::EIM_UIOnly;
	bool bIsVisibleTargetCursor = true;

	if (ActiveAdditiveWidgets.IsEmpty())
	{
		TargetInputMode = CurrentLevelInputMode;
		bIsVisibleTargetCursor = bIsVisibleLevelCursor;
	}
	else
	{
		int32 ActiveCount = ActiveAdditiveWidgets.Num() - 1;

		TargetInputMode = ActiveAdditiveWidgets[ActiveCount]->GetWidgetInputMode();
		bIsVisibleTargetCursor = ActiveAdditiveWidgets[ActiveCount]->GetWidgetCursorMode();
	}

	APlayerController* CurrentPC = GetWorld()->GetFirstPlayerController();
	checkf(IsValid(CurrentPC), TEXT("Player Controller is invalid"));

	switch (TargetInputMode)
	{
	case ESLInputModeType::EIM_UIOnly:
		CurrentPC->SetInputMode(FInputModeUIOnly());
		break;

	case ESLInputModeType::EIM_GameOnly:
		CurrentPC->SetInputMode(FInputModeGameOnly());
		break;

	case ESLInputModeType::EIM_GameAndUI:
		CurrentPC->SetInputMode(FInputModeGameAndUI());
		break;
	}

	CurrentPC->SetShowMouseCursor(bIsVisibleTargetCursor);

	if (!bIsRemove)
	{
		CurrentPC->FlushPressedKeys();
	}
}

void USLUISubsystem::SetChapterToUI(ESLChapterType ChapterType)
{
	WidgetActivateBuffer.CurrentChapter = ChapterType;

	for (USLAdditiveWidget* ActiveWidget : ActiveAdditiveWidgets)
	{
		if (IsValid(ActiveWidget))
		{
			ActiveWidget->ApplyOnChangedChapter(WidgetActivateBuffer);
		}
	}
}

void USLUISubsystem::SetLevelInputMode(ESLInputModeType InputModeType, bool bIsVisibleMouseCursor)
{
	CurrentLevelInputMode = InputModeType;
	bIsVisibleLevelCursor = bIsVisibleMouseCursor;

	RemoveAllAdditveWidget();
}

void USLUISubsystem::ActivateOption()
{
	CheckValidOfOptiondDataAsset();
	AddAdditiveWidget(ESLAdditiveWidgetType::EAW_OptionWidget);
}

void USLUISubsystem::ActivateFade(bool bIsFadeIn, bool bIsMoveLevel)
{
	WidgetActivateBuffer.bIsFade = bIsFadeIn;
	WidgetActivateBuffer.bIsMoveLevel = bIsMoveLevel;

	AddAdditiveWidget(ESLAdditiveWidgetType::EAW_FadeWidget);
}

void USLUISubsystem::ActivateNotify(ESLGameMapType MapType, const FName NotiName)
{
	WidgetActivateBuffer.TargetMap = MapType;
	WidgetActivateBuffer.TargetNotify = NotiName;

	AddAdditiveWidget(ESLAdditiveWidgetType::EAW_NotifyWidget);
}

USLStoryWidget* USLUISubsystem::ActivateStory(ESLStoryType TargetStoryType)
{
	WidgetActivateBuffer.TargetStory = TargetStoryType;

	AddAdditiveWidget(ESLAdditiveWidgetType::EAW_StoryWidget);

	return Cast<USLStoryWidget>(AdditiveWidgetMap[ESLAdditiveWidgetType::EAW_StoryWidget]);
}

USLBaseTextPrintWidget* USLUISubsystem::ActivateTalk(ESLTalkTargetType TalkTargetType, FName TargetName, FName TalkName)
{
	WidgetActivateBuffer.TargetTalk = TalkTargetType;
	WidgetActivateBuffer.TargetName = TargetName;
	WidgetActivateBuffer.TalkName = TalkName;

	AddAdditiveWidget(ESLAdditiveWidgetType::EAW_TalkWidget);

	USLBaseTextPrintWidget* TalkWidget = Cast<USLBaseTextPrintWidget>(AdditiveWidgetMap[ESLAdditiveWidgetType::EAW_TalkWidget]);

	return TalkWidget;
}

void USLUISubsystem::AddAdditiveWidget(ESLAdditiveWidgetType WidgetType)
{
	CheckValidOfAdditiveWidget(WidgetType);

	if (!ActiveAdditiveWidgets.Contains(AdditiveWidgetMap[WidgetType]))
	{
		ActiveAdditiveWidgets.Add(AdditiveWidgetMap[WidgetType]);
	}

	if (!AdditiveWidgetMap[WidgetType]->IsInViewport())
	{
		AdditiveWidgetMap[WidgetType]->AddToViewport(AdditiveWidgetMap[WidgetType]->GetWidgetOrder());
	}

	AdditiveWidgetMap[WidgetType]->ActivateWidget(WidgetActivateBuffer);
	SetInputModeAndCursor(false);
}

void USLUISubsystem::RemoveCurrentAdditiveWidget(ESLAdditiveWidgetType WidgetType)
{
	if (ActiveAdditiveWidgets.IsEmpty())
	{
		return;
	}

	if (AdditiveWidgetMap.Contains(WidgetType) &&
		!ActiveAdditiveWidgets.Contains(AdditiveWidgetMap[WidgetType]))
	{
		return;
	}

	AdditiveWidgetMap[WidgetType]->DeactivateWidget();
	ActiveAdditiveWidgets.Remove(AdditiveWidgetMap[WidgetType]);
	SetInputModeAndCursor(true);
}

void USLUISubsystem::RemoveAllAdditveWidget()
{
	for (USLAdditiveWidget* ActiveWidget : ActiveAdditiveWidgets)
	{
		if (IsValid(ActiveWidget) && ActiveWidget->IsInViewport())
		{
			ActiveWidget->DeactivateWidget();
		}
	}

	ActiveAdditiveWidgets.Empty();
	SetInputModeAndCursor(true);
}

UDataAsset* USLUISubsystem::GetPublicImageData()
{
	CheckValidOfWidgetDataAsset();
	return WidgetActivateBuffer.WidgetPublicData;
}

USLBaseTextPrintWidget* USLUISubsystem::GetTalkWidget()
{
	CheckValidOfAdditiveWidget(ESLAdditiveWidgetType::EAW_TalkWidget);
	USLBaseTextPrintWidget* TalkWidget = Cast<USLBaseTextPrintWidget>(AdditiveWidgetMap[ESLAdditiveWidgetType::EAW_TalkWidget]);

	return TalkWidget;
}

void USLUISubsystem::CheckValidOfAdditiveWidget(ESLAdditiveWidgetType WidgetType)
{
	if (AdditiveWidgetMap.Contains(WidgetType))
	{
		if (IsValid(AdditiveWidgetMap[WidgetType]))
		{
			return;
		}
	}

	CheckValidOfUISettings();
	checkf(UISettings->WidgetClassMap.Contains(WidgetType), TEXT("Widget Class Map is not contains widgettype"));

	UClass* WidgetClass = UISettings->WidgetClassMap[WidgetType].LoadSynchronous();
	checkf(IsValid(WidgetClass), TEXT("WidgetClass is invalid"));

	USLAdditiveWidget* TempWidget = CreateWidget<USLAdditiveWidget>(GetGameInstance(), WidgetClass);
	checkf(IsValid(TempWidget), TEXT("Fail Create Widget"));

	TempWidget->InitWidget(this);
	AdditiveWidgetMap.Add(WidgetType, TempWidget);

	CheckValidOfWidgetDataAsset();
}

void USLUISubsystem::CheckValidOfUISettings()
{
	if (IsValid(UISettings))
	{
		return;
	}

	UISettings = GetDefault<USLUISettings>();
	checkf(IsValid(UISettings), TEXT("UI Settings is invalid"));
}

void USLUISubsystem::CheckValidOfWidgetDataAsset()
{
	if (IsValid(WidgetActivateBuffer.WidgetPublicData) &&
		WidgetActivateBuffer.CurrentChapter == CurrentDataChapter)
	{
		return;
	}

	CurrentDataChapter = WidgetActivateBuffer.CurrentChapter;

	CheckValidOfUISettings();
	
	checkf(UISettings->ChapterWidgetPublicDataMap.Contains(CurrentDataChapter), TEXT("Widget Public Data Map is not contains"));
	WidgetActivateBuffer.WidgetPublicData = UISettings->ChapterWidgetPublicDataMap[CurrentDataChapter].LoadSynchronous();
	checkf(IsValid(WidgetActivateBuffer.WidgetPublicData), TEXT("Widget ImageData is invalid"));
}

void USLUISubsystem::CheckValidOfOptiondDataAsset()
{
	//CheckValidOfWidgetDataAsset();

	if (IsValid(WidgetActivateBuffer.WidgetPrivateData) &&
		WidgetActivateBuffer.CurrentChapter == CurrentOptionDataChapter)
	{
		return;
	}

	CurrentOptionDataChapter = WidgetActivateBuffer.CurrentChapter;
	CheckValidOfUISettings();

	checkf(UISettings->OptionWidgetPrivateDataMap.Contains(CurrentOptionDataChapter), TEXT("Option Private Data Map is not contains"));
	WidgetActivateBuffer.WidgetPrivateData = UISettings->OptionWidgetPrivateDataMap[CurrentOptionDataChapter].LoadSynchronous();
	checkf(IsValid(WidgetActivateBuffer.WidgetPrivateData), TEXT("Option ImageData is invalid"));
}