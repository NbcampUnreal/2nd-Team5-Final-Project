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
#include "UI/Widget/SLWidgetPrivateDataAsset.h"
#include "NiagaraSystem.h"
#include "Components/CanvasPanelSlot.h"

const FName USLTitleWidget::TitleTextIndex = "TitleText";
const FName USLTitleWidget::StartButtonIndex = "StartButton";
const FName USLTitleWidget::OptionButtonIndex = "OptionButton";
const FName USLTitleWidget::QuitButtonIndex = "QuitButton";

void USLTitleWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_GameAndUI;
	bIsVisibleCursor = true;
	
	Super::InitWidget(NewUISubsystem);

	StartButton->InitButton();
	OptionButton->InitButton();
	QuitButton->InitButton();

	StartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedStartButton);
	OptionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedOptionButton);
	QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuitButton);

	//UNiagaraComponent* NiagaraComponent = BackgroundEffect->GetNiagaraComponent();
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

	TMap<FName, FText> OptionTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Title)
		{
			OptionTextMap = UITextPool->TextMap;
			break;
		}
	}

	TitleText->SetText(OptionTextMap[TitleTextIndex]);
	StartButton->SetButtonText(OptionTextMap[StartButtonIndex]);
	OptionButton->SetButtonText(OptionTextMap[OptionButtonIndex]);
	QuitButton->SetButtonText(OptionTextMap[QuitButtonIndex]);
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