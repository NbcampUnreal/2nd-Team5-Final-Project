// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLMainTitleWidget.h"
#include "Components/Image.h"
#include "UI/Widget/SLButtonWidget.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "UI/Widget/SLWidgetPrivateDataAsset.h"
#include "SaveLoad/SLSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystem/SLDemoSubsystem.h"

const FName USLMainTitleWidget::StartButtonIndex = "StartButton";
const FName USLMainTitleWidget::LoadButtonIndex = "ContinueButton";
const FName USLMainTitleWidget::OptionButtonIndex = "OptionButton";
const FName USLMainTitleWidget::QuitButtonIndex = "QuitButton";

void USLMainTitleWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;

	Super::InitWidget(NewUISubsystem);

	StartButton->InitButton();
	LoadButton->InitButton();
	OptionButton->InitButton();
	QuitButton->InitButton();

	StartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedStartButton);
	LoadButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedLoadButton);
	OptionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedOptionButton);
	QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuitButton);
}

void USLMainTitleWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLMainTitleWidget::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::FindWidgetData(WidgetActivateBuffer);

	if (IsValid(WidgetActivateBuffer.WidgetPrivateData))
	{
		USLTitlePrivateDataAsset* PrivateData = Cast<USLTitlePrivateDataAsset>(WidgetActivateBuffer.WidgetPrivateData);
		PrivateImageMap.Empty();
		PrivateImageMap = PrivateData->GetBrushDataMap();
	}
}

void USLMainTitleWidget::ApplyTextData()
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

	StartButton->SetButtonText(TitleTextMap[StartButtonIndex]);
	LoadButton->SetButtonText(TitleTextMap[LoadButtonIndex]);
	OptionButton->SetButtonText(TitleTextMap[OptionButtonIndex]);
	QuitButton->SetButtonText(TitleTextMap[QuitButtonIndex]);
}

bool USLMainTitleWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	FSlateBrush SlateBrush;

	if (PrivateImageMap.Contains(ESLTitlePrivateImageType::ETPI_Background) &&
		IsValid(PrivateImageMap[ESLTitlePrivateImageType::ETPI_Background]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLTitlePrivateImageType::ETPI_Background]);
		BackgroundImg->SetBrush(SlateBrush);
	}

	if (PrivateImageMap.Contains(ESLTitlePrivateImageType::ETPI_Logo) &&
		IsValid(PrivateImageMap[ESLTitlePrivateImageType::ETPI_Logo]))
	{
		SlateBrush.SetResourceObject(PrivateImageMap[ESLTitlePrivateImageType::ETPI_Logo]);
		LogoImg->SetBrush(SlateBrush);
	}

	return true;
}

void USLMainTitleWidget::OnClickedStartButton()
{
	PlayUISound(ESLUISoundType::EUS_Click);

	USLSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>();
	checkf(IsValid(SaveGameSubsystem), TEXT("Save Game Subsystem is invalid"));

	SaveGameSubsystem->ResetGameData();

	USLDemoSubsystem* DemoSub = GetGameInstance()->GetSubsystem<USLDemoSubsystem>();
	checkf(IsValid(DemoSub), TEXT("Demo Sub is invalid"));
	DemoSub->StartNewGame();

	MoveToLevelByType(ESLLevelNameType::ELN_Title);
}

void USLMainTitleWidget::OnClickedLoadButton()
{
	CheckValidOfUISubsystem();
	UISubsystem->ActivateSaveLoadWidget();
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLMainTitleWidget::OnClickedOptionButton()
{
	CheckValidOfUISubsystem();
	UISubsystem->ActivateOption();
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLMainTitleWidget::OnClickedQuitButton()
{
	USLDemoSubsystem* DemoSub = GetGameInstance()->GetSubsystem<USLDemoSubsystem>();
	checkf(IsValid(DemoSub), TEXT("Demo Sub is invalid"));
	DemoSub->EndCurrentGame();

	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
	PlayUISound(ESLUISoundType::EUS_Click);
}
