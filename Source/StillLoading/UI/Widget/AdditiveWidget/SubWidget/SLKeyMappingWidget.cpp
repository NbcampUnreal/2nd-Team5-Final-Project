// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLKeyMappingWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"


void USLKeyMappingWidget::InitWidget(const FName& NewTagText, EInputActionType NewActionType)
{
	ActionType = NewActionType;
	ChangeButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedChangeKey);
	UpdateTagText(NewTagText);
}

void USLKeyMappingWidget::UpdateTagText(const FName& NewTagText)
{
	TagText->SetText(FText::FromName(NewTagText));
}

void USLKeyMappingWidget::UpdateKeyText(const FName& KeyText)
{
	KeyTextBox->SetText(FText::FromName(KeyText));
}

void USLKeyMappingWidget::SetVisibilityButton(bool bIsVisible)
{
	if (bIsVisible)
	{
		ChangeButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ChangeButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USLKeyMappingWidget::SetIsEnabledButton(bool bIsEnable)
{
	ChangeButton->SetIsEnabled(bIsEnable);
}

void USLKeyMappingWidget::OnClickedChangeKey()
{
	KeyDelegate.Broadcast(ActionType);
}
