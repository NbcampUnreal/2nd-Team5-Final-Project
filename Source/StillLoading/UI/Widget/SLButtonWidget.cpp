// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLButtonWidget.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/RetainerBox.h"
#include "NiagaraSystemWidget.h"
#include "NiagaraSystem.h"
#include "NiagaraUIComponent.h"
#include "NiagaraFunctionLibrary.h"

void USLButtonWidget::InitButton()
{
	Button->OnClicked.AddDynamic(this, &ThisClass::OnClickedButton);
	Button->OnHovered.AddDynamic(this, &ThisClass::OnHoveredButton);
	Button->OnUnhovered.AddDynamic(this, &ThisClass::OnUnhoveredButton);
}

void USLButtonWidget::SetButtonStyle(FButtonStyle NewStyle)
{
	UMaterialInterface* ButtonMaterial = Cast<UMaterialInterface>(NewStyle.Hovered.GetResourceObject());

	if (IsValid(ButtonMaterial))
	{
		ButtonDynamicMat = UMaterialInstanceDynamic::Create(ButtonMaterial, this);
		NewStyle.Hovered.SetResourceObject(ButtonDynamicMat);
	}
	else
	{
		ButtonDynamicMat = nullptr;
	}

	Button->SetStyle(NewStyle);
}

void USLButtonWidget::SetButtonText(const FText& NewText)
{
	TextBlock->SetText(NewText);
}

void USLButtonWidget::SetButtonFont(FSlateFontInfo& NewFont, float FontOffset)
{
	NewFont.Size = TextBlock->GetFont().Size;
	TextBlock->SetFont(NewFont);
	TextBlock->SetRenderTranslation(FVector2D(0, FontOffset));
}

void USLButtonWidget::SetRetainerMat(UMaterialInterface* NewMaterial)
{
	if (IsValid(NewMaterial))
	{
		RetainerBox->SetEffectMaterial(NewMaterial);
	}
}

void USLButtonWidget::SetNiagaraWidget(UNiagaraSystem* NewNiagara)
{
	if (IsValid(NewNiagara))
	{
		NiagaraWidget->UpdateNiagaraSystemReference(NewNiagara);
		bIsContainNiagara = true;
		NiagaraWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USLButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RetainerBox->SetRetainRendering(false);
}

void USLButtonWidget::OnClickedButton()
{
	PlayButtonSound(ESLUISoundType::EUS_Click);
	OnClicked.Broadcast();
}

void USLButtonWidget::OnHoveredButton()
{
	PlayButtonSound(ESLUISoundType::EUS_Hover);

	RetainerBox->SetRetainRendering(true);
	PlayAnimation(AddProgress);

	if (bIsContainNiagara)
	{
		NiagaraWidget->ActivateSystem(true);
		NiagaraWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void USLButtonWidget::OnUnhoveredButton()
{
	RetainerBox->SetRetainRendering(false);

	if (bIsContainNiagara)
	{
		NiagaraWidget->GetNiagaraComponent()->DeactivateImmediate();
		NiagaraWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USLButtonWidget::PlayButtonSound(ESLUISoundType SoundType)
{
	USLSoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USLSoundSubsystem>();
	checkf(IsValid(SoundSubsystem), TEXT("Sound Subsystem is invalid"));
	SoundSubsystem->PlayUISound(SoundType);
}