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
#include "NiagaraFunctionLibrary.h"
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

	UNiagaraComponent* NiagaraComponent = BackgroundEffect->GetNiagaraComponent();
	
	//FNiagaraDataSetIterator<FVector> ParticlePos;
	//NiagaraComponent->GetNiagaraDataInterface("PositionData")->GetDataSet()->GetFloatData(ParticlePos);

	//UNiagaraComponent* NiagaraComponent = BackgroundEffect->GetNiagaraComponent();
	//
	//UNiagaraFunctionLibrary::GetNiagaraParticle

	//for (int i = 0; i < ParticlePos.Num(); ++i)
	//{
	//	FVector2D Pos2D = ProjectToWidgetSpace(ParticlePos[i]); // 필요 시 Viewport 변환
	//	// Pos2D 값을 저장
	//}
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
	StartText->SetText(OptionTextMap[StartButtonIndex]);
	OptionText->SetText(OptionTextMap[OptionButtonIndex]);
	QuitText->SetText(OptionTextMap[QuitButtonIndex]);
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
	Super::ApplyButtonImage(ButtonStyle);

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
	PlayHoverSound();

	if (!bIsContainEffect)
	{
		return;
	}

	StartButtonEffect->ActivateSystem(true);
	StartButtonEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USLTitleWidget::OnHoveredOptionButton()
{
	PlayHoverSound();

	if (!bIsContainEffect)
	{
		return;
	}

	OptionButtonEffect->ActivateSystem(true);
	OptionButtonEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USLTitleWidget::OnHoveredQuitButton()
{
	PlayHoverSound();

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
