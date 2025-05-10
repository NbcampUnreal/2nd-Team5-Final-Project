// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLKeyMappingWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"


void USLKeyMappingWidget::InitWidget(const FName& NewTagText, const FKey& NewKey)
{
	UpdateTagText(NewTagText);
	UpdateKeyText(NewKey);

	ChangeButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedChangeKey);
}

void USLKeyMappingWidget::UpdateTagText(const FName& NewTagText)
{
	TagText->SetText(FText::FromName(NewTagText));
}

void USLKeyMappingWidget::UpdateKeyText(const FKey& NewKey)
{
	InputKey = NewKey;
	KeyTextBox->SetText(FText::FromString(InputKey.ToString()));
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
	KeyDelegate.Broadcast(InputKey);
}
