// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLPauseWidget.h"
#include "SubSystem/SLLevelTransferSubsystem.h"
#include "SubSystem/SLDemoSubsystem.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "GameMode/SLGameModeBase.h"
#include "SaveLoad/SLSaveGameSubsystem.h"
#include "UI/Widget/SLButtonWidget.h"
#include "UI/HUD/SLBaseHUD.h"
#include "Components/Image.h"

void USLPauseWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_PauseWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 15;
	bIsVisibleCursor = true;
	bIsFocusable = true;

	Super::InitWidget(NewUISubsystem);

	RestartButton->InitButton();
	CloseButton->InitButton();
	TitleButton->InitButton();

	CloseButton->SetButtonText(FText::FromString(FString::Printf(TEXT("게임 재개"))));
	RestartButton->SetButtonText(FText::FromString(FString::Printf(TEXT("다시 시작"))));
	TitleButton->SetButtonText(FText::FromString(FString::Printf(TEXT("게임 종료"))));

	RestartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedRestart);
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);
	TitleButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedTitle);
}

void USLPauseWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);
	SetFocus();
}

void USLPauseWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

FReply USLPauseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!IsInViewport())
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	FKey OptionKey = InKeyEvent.GetKey();
	USLUserDataSubsystem* UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();

	if (UserDataSubsystem->GetActionKeyMap().Contains(EInputActionType::EIAT_Menu))
	{
		if (OptionKey == UserDataSubsystem->GetActionKeyMap()[EInputActionType::EIAT_Menu].Key)
		{
			CloseWidget();
		}
	}

	return FReply::Handled();
}

bool USLPauseWidget::ApplyOtherImage()
{
	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_PauseBack) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_PauseBack]))
	{
		return false;
	}
	
	FSlateBrush SlateBrush;

	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_PauseBack]);
	BackgroundImg->SetBrush(SlateBrush);

	if (!PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_Logo) ||
		!IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Logo]))
	{
		return false;
	}

	SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Logo]);
	LogoImg->SetBrush(SlateBrush);

	return true;
}

void USLPauseWidget::OnEndedCloseAnim()
{
	Super::OnEndedCloseAnim();

	APlayerController* PC = GetWorld()->GetPlayerControllerIterator()->Get();
	checkf(IsValid(PC), TEXT("PlayerController is invalid"));

	ASLBaseHUD* HUD = Cast<ASLBaseHUD>(PC->GetHUD());
	checkf(IsValid(HUD), TEXT("HUD is invalid"));

	HUD->OnUnpause();
}

void USLPauseWidget::OnClickedRestart()
{
	PlayUISound(ESLUISoundType::EUS_Click);

	ASLGameModeBase* GM = Cast<ASLGameModeBase>(GetWorld()->GetAuthGameMode());

	if (IsValid(GM))
	{
		GM->ResetModifiedObjectives();
	}

	USLDemoSubsystem* DemoSub = GetGameInstance()->GetSubsystem<USLDemoSubsystem>();
	checkf(IsValid(DemoSub), TEXT("DemoSub is invalid"));
	DemoSub->ResetCurrentInfo();

	USLLevelTransferSubsystem* LevelTransferSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
	checkf(IsValid(LevelTransferSubsystem), TEXT("Level Transfer Subsystem is invalid"));

	ESLLevelNameType CurrentLevelType = LevelTransferSubsystem->GetCurrentLevelType();
	LevelTransferSubsystem->OpenLevelByNameType(CurrentLevelType);
}

void USLPauseWidget::OnClickedTitle()
{
	PlayUISound(ESLUISoundType::EUS_Click);
	CloseWidget();

	USLSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>();
	checkf(IsValid(SaveGameSubsystem), TEXT("SaveGameSubsystem is invalid"));
	SaveGameSubsystem->SaveSettingData();

	USLDemoSubsystem* DemoSub = GetGameInstance()->GetSubsystem<USLDemoSubsystem>();
	checkf(IsValid(DemoSub), TEXT("DemoSub is invalid"));
	DemoSub->EndCurrentGame();

	USLLevelTransferSubsystem* LevelTransferSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
	checkf(IsValid(LevelTransferSubsystem), TEXT("Level Transfer Subsystem is invalid"));
	LevelTransferSubsystem->MoveToMainTitle();
}
