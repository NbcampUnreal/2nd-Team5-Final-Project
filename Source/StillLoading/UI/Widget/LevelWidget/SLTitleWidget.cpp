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
#include "NiagaraSystemWidget.h"
#include "NiagaraSystem.h"
#include "NiagaraUIComponent.h"
#include "NiagaraFunctionLibrary.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Components/CanvasPanelSlot.h"
#include "Components/RetainerBox.h"

const FName USLTitleWidget::TitleTextIndex = "TitleText";
const FName USLTitleWidget::StartButtonIndex = "StartButton";
const FName USLTitleWidget::OptionButtonIndex = "OptionButton";
const FName USLTitleWidget::QuitButtonIndex = "QuitButton";

void USLTitleWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_GameAndUI;
	bIsVisibleCursor = true;
	
	Super::InitWidget(NewUISubsystem);

	StartButtonWidget->OnClicked.AddDynamic(this, &ThisClass::OnClickedStartButton);
	StartButtonWidget->InitButton();

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

	StartRetainer->SetRetainRendering(false);
	OptionRetainer->SetRetainRendering(false);
	QuitRetainer->SetRetainRendering(false);
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

	StartButtonWidget->SetButtonText(OptionTextMap[StartButtonIndex]);

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
			StartButtonWidget->SetNiagaraWidget(Niagara);

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

	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_ButtonRetainer) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonRetainer]))
	{
		UMaterialInterface* EffectMat = Cast<UMaterialInterface>(PublicAssetMap[ESLPublicWidgetImageType::EPWI_ButtonRetainer]);

		if (IsValid(EffectMat))
		{
			StartButtonWidget->SetRetainerMat(EffectMat);

			StartRetainer->SetEffectMaterial(EffectMat);
			OptionRetainer->SetEffectMaterial(EffectMat);
			QuitRetainer->SetEffectMaterial(EffectMat);
		}
	}

	StartButtonWidget->SetButtonStyle(ButtonStyle);

	StartButton->SetStyle(ButtonStyle);
	OptionButton->SetStyle(ButtonStyle);
	QuitButton->SetStyle(ButtonStyle);

	MappingButtonMaterial();

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
	StartRetainer->SetRetainRendering(true);

	if (ButtonDMIMap.Contains(StartButton) &&
		IsValid(ButtonDMIMap[StartButton]))
	{
		CheckButtonMaterial(ButtonDMIMap[StartButton]);
	}

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
	OptionRetainer->SetRetainRendering(true);

	if (ButtonDMIMap.Contains(OptionButton) &&
		IsValid(ButtonDMIMap[OptionButton]))
	{
		CheckButtonMaterial(ButtonDMIMap[OptionButton]);
	}

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
	QuitRetainer->SetRetainRendering(true);

	if (ButtonDMIMap.Contains(QuitButton) &&
		IsValid(ButtonDMIMap[QuitButton]))
	{
		CheckButtonMaterial(ButtonDMIMap[QuitButton]);
	}

	if (!bIsContainEffect)
	{
		return;
	}

	QuitButtonEffect->ActivateSystem(true);
	QuitButtonEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USLTitleWidget::OnUnhorveredButton()
{
	GetWorld()->GetTimerManager().ClearTimer(HoverTimer);
	StartRetainer->SetRetainRendering(false);
	OptionRetainer->SetRetainRendering(false);
	QuitRetainer->SetRetainRendering(false);

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

void USLTitleWidget::MappingButtonMaterial()
{
	ButtonDMIMap.Empty();

	UMaterialInterface* ButtonMaterial = Cast<UMaterialInterface>(StartButton->GetStyle().Hovered.GetResourceObject());
	UMaterialInstanceDynamic* ButtonDynamicMat = nullptr;
	FButtonStyle ButtonStyle;

	if (IsValid(ButtonMaterial))
	{
		ButtonDynamicMat = UMaterialInstanceDynamic::Create(ButtonMaterial, this);
		ButtonStyle = StartButton->GetStyle();
		ButtonStyle.Hovered.SetResourceObject(ButtonDynamicMat);
		StartButton->SetStyle(ButtonStyle);
		ButtonDMIMap.Add(StartButton, ButtonDynamicMat);
	}

	ButtonMaterial = Cast<UMaterialInterface>(OptionButton->GetStyle().Hovered.GetResourceObject());

	if (IsValid(ButtonMaterial))
	{
		ButtonDynamicMat = UMaterialInstanceDynamic::Create(ButtonMaterial, this);
		ButtonStyle = OptionButton->GetStyle();
		ButtonStyle.Hovered.SetResourceObject(ButtonDynamicMat);
		OptionButton->SetStyle(ButtonStyle);
		ButtonDMIMap.Add(OptionButton, ButtonDynamicMat);
	}

	ButtonMaterial = Cast<UMaterialInterface>(QuitButton->GetStyle().Hovered.GetResourceObject());

	if (IsValid(ButtonMaterial))
	{
		ButtonDynamicMat = UMaterialInstanceDynamic::Create(ButtonMaterial, this);
		ButtonStyle = QuitButton->GetStyle();
		ButtonStyle.Hovered.SetResourceObject(ButtonDynamicMat);
		QuitButton->SetStyle(ButtonStyle);
		ButtonDMIMap.Add(QuitButton, ButtonDynamicMat);
	}
}

void USLTitleWidget::CheckButtonMaterial(UMaterialInstanceDynamic* ButtonMaterial)
{
	DynamicMat = ButtonMaterial;

	if (IsValid(DynamicMat))
	{
		TArray<FMaterialParameterInfo> ParameterInfos;
		TArray<FGuid> ParameterIds;

		DynamicMat->GetAllScalarParameterInfo(ParameterInfos, ParameterIds);

		bool bIsContain = false;

		for (const FMaterialParameterInfo& ParameterInfo : ParameterInfos)
		{
			if (ParameterInfo.Name == ProgressName)
			{
				bIsContain = true;
				break;
			}
		}

		if (bIsContain)
		{
			PlayHoverEvent();
		}
	}
}

void USLTitleWidget::PlayHoverEvent()
{
	CurrentProgress = 0.0f;
	DynamicMat->SetScalarParameterValue(ProgressName, CurrentProgress);
	SetHoverTimer();
}

void USLTitleWidget::SetHoverTimer()
{
	DynamicMat->GetScalarParameterValue(ProgressName, CurrentProgress);

	if (CurrentProgress < 1.0f)
	{
		CurrentProgress += ProgressValue;
		DynamicMat->SetScalarParameterValue(ProgressName, CurrentProgress);

		GetWorld()->GetTimerManager().SetTimer(HoverTimer, this, &ThisClass::SetHoverTimer, ProgressDuration, false);
	}
}
