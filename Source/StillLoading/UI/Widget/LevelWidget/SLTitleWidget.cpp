// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLTitleWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SLUISubsystem.h"
#include "UI/Widget/SLButtonWidget.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SaveLoad/SLSaveGameSubsystem.h"
#include "UI/Widget/SLWidgetPrivateDataAsset.h"
#include "NiagaraSystem.h"

const FName USLTitleWidget::TitleTextIndex = "TitleText";
const FName USLTitleWidget::StartButtonIndex = "StartButton";
const FName USLTitleWidget::ContinueButtonIndex = "ContinueButton";
const FName USLTitleWidget::OptionButtonIndex = "OptionButton";
const FName USLTitleWidget::QuitButtonIndex = "QuitButton";
const FName USLTitleWidget::QuestionTextIndex = "QuestionText";

void USLTitleWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_GameAndUI;
	bIsVisibleCursor = true;
	
	Super::InitWidget(NewUISubsystem);

	StartButton->InitButton();
	ContinueButton->InitButton();
	OptionButton->InitButton();
	QuitButton->InitButton();
	AgreeButton->InitButton();
	CancleButton->InitButton();

	StartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedNewGameButton);
	ContinueButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedContinueButton);
	OptionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedOptionButton);
	QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuitButton);
	AgreeButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedStartButton);
	CancleButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedCancleButton);

	CheckValidOfSaveGameSubsystem();
	
	if (SaveGameSubsystem->GetIsExistSaveData())
	{
		ContinueButton->SetIsEnabled(true);
	}
	else
	{
		ContinueButton->SetIsEnabled(false);
	}

	QuestionPanel->SetVisibility(ESlateVisibility::Collapsed);
	AgreeButton->SetButtonText(FText::FromString(FString::Printf(TEXT("Yes"))));
	CancleButton->SetButtonText(FText::FromString(FString::Printf(TEXT("No"))));
}

void USLTitleWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLTitleWidget::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::FindWidgetData(WidgetActivateBuffer);

	if (IsValid(WidgetActivateBuffer.WidgetPrivateData))
	{
		USLTitlePrivateDataAsset* PrivateData = Cast<USLTitlePrivateDataAsset>(WidgetActivateBuffer.WidgetPrivateData);
		PrivateImageMap.Empty();
		PrivateImageMap = PrivateData->GetBrushDataMap();
		NextLevelType = PrivateData->GetStartLevelType();
	}
}

void USLTitleWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FText> TitleTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Title)
		{
			TitleTextMap = UITextPool->TextMap;
			break;
		}
	}

	TitleText->SetText(TitleTextMap[TitleTextIndex]);
	StartButton->SetButtonText(TitleTextMap[StartButtonIndex]);
	ContinueButton->SetButtonText(TitleTextMap[ContinueButtonIndex]);
	OptionButton->SetButtonText(TitleTextMap[OptionButtonIndex]);
	QuitButton->SetButtonText(TitleTextMap[QuitButtonIndex]);
	QuestionText->SetText(TitleTextMap[QuestionTextIndex]);
}

bool USLTitleWidget::ApplyBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyBorderImage(SlateBrush))
	{
		return false;
	}

	QuestionBack->SetBrush(SlateBrush);

	return true;
}

bool USLTitleWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	FSlateBrush SlateBrush;

	if (PrivateImageMap.Contains(ESLTitlePrivateImageType::ETPI_Background) &&
		IsValid(PrivateImageMap[ESLTitlePrivateImageType::ETPI_Background]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLTitlePrivateImageType::ETPI_Background]);
		BackgroundBorder->SetBrush(SlateBrush);
	}

	if (PrivateImageMap.Contains(ESLTitlePrivateImageType::ETPI_Logo) &&
		IsValid(PrivateImageMap[ESLTitlePrivateImageType::ETPI_Logo]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLTitlePrivateImageType::ETPI_Logo]);
		TitleTextImg->SetBrush(SlateBrush);
		TitleText->SetVisibility(ESlateVisibility::Collapsed);
	}

	return true;
}

void USLTitleWidget::OnClickedStartButton()
{
	CheckValidOfSaveGameSubsystem();
	SaveGameSubsystem->ResetGameData();

	PlayUISound(ESLUISoundType::EUS_Click);
	MoveToLevelByType(ESLLevelNameType::ELN_Intro);
}

void USLTitleWidget::OnClickedContinueButton()
{
	PlayUISound(ESLUISoundType::EUS_Click);
	MoveToLevelByType(NextLevelType);
}

void USLTitleWidget::OnClickedOptionButton()
{
	CheckValidOfUISubsystem();
	UISubsystem->ActivateOption();
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLTitleWidget::OnClickedQuitButton()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLTitleWidget::OnClickedNewGameButton()
{
	QuestionPanel->SetVisibility(ESlateVisibility::Visible);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLTitleWidget::OnClickedCancleButton()
{
	QuestionPanel->SetVisibility(ESlateVisibility::Collapsed);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLTitleWidget::CheckValidOfSaveGameSubsystem()
{
	if (IsValid(SaveGameSubsystem))
	{
		return;
	}

	SaveGameSubsystem = GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>();
	checkf(IsValid(SaveGameSubsystem), TEXT("SaveGameSubsystem is invalid"));
}
