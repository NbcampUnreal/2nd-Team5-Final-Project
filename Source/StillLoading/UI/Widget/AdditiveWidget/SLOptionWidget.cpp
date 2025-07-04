// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLOptionWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/SLButtonWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLKeySettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLLanguageSettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLGraphicSettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLSoundSettingWidget.h"
#include "Animation/WidgetAnimation.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/SLBaseHUD.h"
#include "UI/Widget/SLWidgetPrivateDataAsset.h"
#include "NiagaraSystem.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SubSystem/SLLevelTransferSubsystem.h"
#include "SaveLoad/SLSaveGameSubsystem.h"
#include "GameMode/SLGameModeBase.h"

const FName USLOptionWidget::TitleTextIndex = "TitleText";
const FName USLOptionWidget::KeySettingButtonIndex = "KeySettingButton";
const FName USLOptionWidget::LanguageSettingIndex = "LanguageSetting";
const FName USLOptionWidget::GraphicSettingIndex = "GraphicSetting";
const FName USLOptionWidget::SoundSettingIndex = "SoundSetting";
const FName USLOptionWidget::RestartButtonIndex = "RestartButton";
const FName USLOptionWidget::MoveToTitleButtonIndex = "MoveToTitleButton";
const FName USLOptionWidget::CloseButtonIndex = "CloseButton";
const FName USLOptionWidget::SettingResetButtonIndex = "SettingResetButton";

void USLOptionWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_OptionWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 15;
	bIsVisibleCursor = true;
	bIsFocusable = true;
	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem);

	LanguageSetBt->InitButton();
	GraphicSetBt->InitButton();
	SoundSetBt->InitButton();
	KeySettingButton->InitButton();

	RestartButton->InitButton();
	MoveToTitleButton->InitButton();
	CloseButton->InitButton();
	SettingResetButton->InitButton();

	LanguageSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedLanguageSetting);
	GraphicSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedGraphicSetting);
	SoundSetBt->OnClicked.AddDynamic(this, &ThisClass::OnClickedSoundSetting);
	KeySettingButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedKeySetting);

	RestartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedRestart);
	MoveToTitleButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedMoveToTitle);
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);
	SettingResetButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedSettingReset);

	LanguageSettingWidget->InitWidget(NewUISubsystem);
	GraphicSettingWidget->InitWidget(NewUISubsystem);
	SoundSettingWidget->InitWidget(NewUISubsystem);
	KeySettingWidget->InitWidget(NewUISubsystem);
}

void USLOptionWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	SetFocus();

	if (IsValid(OpenAnim))
	{
		PlayAnimation(OpenAnim);
	}
	else
	{
		OnEndedOpenAnim();
	}

	LayerSwitcher->SetActiveWidgetIndex(0);
	
	LanguageSettingWidget->ActivateWidget(WidgetActivateBuffer);
	GraphicSettingWidget->ActivateWidget(WidgetActivateBuffer);
	SoundSettingWidget->ActivateWidget(WidgetActivateBuffer);
	KeySettingWidget->ActivateWidget(WidgetActivateBuffer);

	PlayUISound(ESLUISoundType::EUS_Open);

	CheckValidOfLevelTransferSubsystem();

	if (LevelTransferSubsystem->GetCurrentLevelType() == ESLLevelNameType::ELN_Title)
	{
		MoveToTitleButton->SetVisibility(ESlateVisibility::Collapsed);
		RestartButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		MoveToTitleButton->SetVisibility(ESlateVisibility::Visible);
		RestartButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void USLOptionWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	LanguageSettingWidget->DeactivateWidget();
	GraphicSettingWidget->DeactivateWidget();
	SoundSettingWidget->DeactivateWidget();
	KeySettingWidget->DeactivateWidget();

	if (IsValid(CloseAnim))
	{
		PlayAnimation(CloseAnim);
	}
	else
	{
		OnEndedCloseAnim();
	}

	PlayUISound(ESLUISoundType::EUS_Close);
}

void USLOptionWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FText> OptionTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Option)
		{
			OptionTextMap = UITextPool->TextMap;
			break;
		}
	}

	TitleText->SetText(OptionTextMap[TitleTextIndex]);

	LanguageSetBt->SetButtonText(OptionTextMap[LanguageSettingIndex]);
	GraphicSetBt->SetButtonText(OptionTextMap[GraphicSettingIndex]);
	SoundSetBt->SetButtonText(OptionTextMap[SoundSettingIndex]);
	KeySettingButton->SetButtonText(OptionTextMap[KeySettingButtonIndex]);

	RestartButton->SetButtonText(OptionTextMap[RestartButtonIndex]);
	MoveToTitleButton->SetButtonText(OptionTextMap[MoveToTitleButtonIndex]);
	CloseButton->SetButtonText(OptionTextMap[CloseButtonIndex]);
	SettingResetButton->SetButtonText(OptionTextMap[SettingResetButtonIndex]);
}

bool USLOptionWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	if (PrivateImageMap.Contains(ESLOptionPrivateImageType::EOPI_Background) &&
		IsValid(PrivateImageMap[ESLOptionPrivateImageType::EOPI_Background]))
	{
		FSlateBrush SlateBrush;
		SlateBrush.SetResourceObject(PrivateImageMap[ESLOptionPrivateImageType::EOPI_Background]);
		OptionPanelBack->SetBrush(SlateBrush);
	}

	return true;
}

void USLOptionWidget::OnEndedCloseAnim()
{
	Super::OnEndedCloseAnim();

	APlayerController* PC = GetWorld()->GetPlayerControllerIterator()->Get();
	checkf(IsValid(PC), TEXT("PlayerController is invalid"));

	ASLBaseHUD* HUD = Cast<ASLBaseHUD>(PC->GetHUD());
	checkf(IsValid(HUD), TEXT("HUD is invalid"));

	HUD->OnUnpause();
}

void USLOptionWidget::OnClickedLanguageSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(0);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedGraphicSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(1);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedSoundSetting()
{
	LayerSwitcher->SetActiveWidgetIndex(2);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedKeySetting()
{
	LayerSwitcher->SetActiveWidgetIndex(3);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLOptionWidget::OnClickedMoveToTitle()
{
	PlayUISound(ESLUISoundType::EUS_Click);

	USLSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>();
	checkf(IsValid(SaveGameSubsystem), TEXT("SaveGameSubsystem is invalid"));
	SaveGameSubsystem->SaveUserData();
	SaveGameSubsystem->LoadGameData();

	CheckValidOfLevelTransferSubsystem();
	LevelTransferSubsystem->OpenLevelByNameType(ESLLevelNameType::ELN_Title);
}

void USLOptionWidget::OnClickedRestart()
{
	PlayUISound(ESLUISoundType::EUS_Click);

	ASLGameModeBase* GM = Cast<ASLGameModeBase>(GetWorld()->GetAuthGameMode());

	if (IsValid(GM))
	{
		GM->ResetModifiedObjectives();
	}

	CheckValidOfLevelTransferSubsystem();
	ESLLevelNameType CurrentLevelType = LevelTransferSubsystem->GetCurrentLevelType();
	LevelTransferSubsystem->OpenLevelByNameType(CurrentLevelType);
}

void USLOptionWidget::OnClickedSettingReset()
{
	USLUserDataSubsystem* UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();

	if (IsValid(UserDataSubsystem))
	{
		UserDataSubsystem->ApplyDefaultUserData();

		LanguageSettingWidget->OnUpdatedSettingValue();
		GraphicSettingWidget->OnUpdatedSettingValue();
		SoundSettingWidget->OnUpdatedSettingValue();
		KeySettingWidget->OnUpdatedSettingValue();
	}
}

FReply USLOptionWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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

void USLOptionWidget::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::FindWidgetData(WidgetActivateBuffer);

	if (IsValid(WidgetActivateBuffer.WidgetPrivateData))
	{
		USLOptionPrivateDataAsset* PrivateData = Cast<USLOptionPrivateDataAsset>(WidgetActivateBuffer.WidgetPrivateData);
		PrivateImageMap.Empty();
		PrivateImageMap = PrivateData->GetBrushDataMap();
	}
}

void USLOptionWidget::CheckValidOfLevelTransferSubsystem()
{
	if (IsValid(LevelTransferSubsystem))
	{
		return;
	}

	LevelTransferSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
	checkf(IsValid(LevelTransferSubsystem), TEXT("LevelTransferSubsystem is invalid"));
}