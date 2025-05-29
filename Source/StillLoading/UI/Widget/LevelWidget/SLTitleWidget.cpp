// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLTitleWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "NiagaraSystemWidget.h"
#include "NiagaraSystem.h"
#include "NiagaraUIComponent.h"
#include <Blueprint/WidgetLayoutLibrary.h>
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

	StartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedStartButton);
	OptionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedOptionButton);
	QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuitButton);

	StartButton->OnHovered.AddDynamic(this, &ThisClass::OnHoveredStartButton);
	StartButton->OnUnhovered.AddDynamic(this, &ThisClass::OnUnhorveredButton);
	OptionButton->OnHovered.AddDynamic(this, &ThisClass::OnHoveredOptionButton);
	OptionButton->OnUnhovered.AddDynamic(this, &ThisClass::OnUnhorveredButton);
	QuitButton->OnHovered.AddDynamic(this, &ThisClass::OnHoveredQuitButton);
	QuitButton->OnUnhovered.AddDynamic(this, &ThisClass::OnUnhorveredButton);
}

void USLTitleWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLTitleWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FSLUITextData> OptionTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Title)
		{
			OptionTextMap = UITextPool->TextMap;
			break;
		}
	}

	TitleText->SetText(OptionTextMap[TitleTextIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	StartText->SetText(OptionTextMap[StartButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	OptionText->SetText(OptionTextMap[OptionButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	QuitText->SetText(OptionTextMap[QuitButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
}

bool USLTitleWidget::ApplyBackgroundImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyBackgroundImage(SlateBrush))
	{
		return false;
	}

	BackgroundBorder->SetBrush(SlateBrush);

	return true;
}

bool USLTitleWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyButtonImage(ButtonStyle))
	{
		FSlateBrush SlateBrush;
		SlateBrush.TintColor = FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
		ButtonStyle.SetNormal(SlateBrush);
		ButtonStyle.SetHovered(SlateBrush);
		ButtonStyle.SetPressed(SlateBrush);
		ButtonStyle.SetDisabled(SlateBrush);
		//return false;
	}

	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ButtonEffect) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonEffect]))
	{
		UNiagaraSystem* Niagara = Cast<UNiagaraSystem>(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonEffect]);

		if (IsValid(Niagara))
		{
			StartButtonEffect->UpdateNiagaraSystemReference(Niagara);
			OptionButtonEffect->UpdateNiagaraSystemReference(Niagara);
			QuitButtonEffect->UpdateNiagaraSystemReference(Niagara);
			bIsContainEffect = true;
		}
	}
	else
	{
		bIsContainEffect = false;
	}

	StartButton->SetStyle(ButtonStyle);
	OptionButton->SetStyle(ButtonStyle);
	QuitButton->SetStyle(ButtonStyle);

	return true;
}

bool USLTitleWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_Logo) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Logo]))
	{
		FSlateBrush SlateBrush;
		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_Logo]);
		TitleTextImg->SetBrush(SlateBrush);
		TitleText->SetVisibility(ESlateVisibility::Collapsed);
	}

	return true;
}

void USLTitleWidget::OnClickedStartButton()
{
	PlayUISound(ESLUISoundType::EUS_Click);
	MoveToLevelByType(ESLLevelNameType::ELN_MapList);
}

void USLTitleWidget::OnClickedOptionButton()
{
	RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType::EAW_OptionWidget);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLTitleWidget::OnClickedQuitButton()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLTitleWidget::OnHoveredStartButton()
{
	if (!bIsContainEffect)
	{
		return;
	}

	StartButtonEffect->ActivateSystem(true);
	StartButtonEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USLTitleWidget::OnHoveredOptionButton()
{
	if (!bIsContainEffect)
	{
		return;
	}

	OptionButtonEffect->ActivateSystem(true);
	OptionButtonEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USLTitleWidget::OnHoveredQuitButton()
{
	if (!bIsContainEffect)
	{
		return;
	}

	QuitButtonEffect->ActivateSystem(true);
	QuitButtonEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USLTitleWidget::OnUnhorveredButton()
{
	if (!bIsContainEffect)
	{
		return;
	}

	StartButtonEffect->GetNiagaraComponent()->DeactivateImmediate();
	StartButtonEffect->SetVisibility(ESlateVisibility::Collapsed);

	OptionButtonEffect->GetNiagaraComponent()->DeactivateImmediate();
	OptionButtonEffect->SetVisibility(ESlateVisibility::Collapsed);

	QuitButtonEffect->GetNiagaraComponent()->DeactivateImmediate();
	QuitButtonEffect->SetVisibility(ESlateVisibility::Collapsed);
}
