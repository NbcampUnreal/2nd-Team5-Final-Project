// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLKeyMappingWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "SubSystem/SLSoundSubsystem.h"


void USLKeyMappingWidget::InitWidget(EInputActionType NewActionType, const FName& NewTagIndex, const FName& KeyText)
{
	ActionType = NewActionType;
	ChangeButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedChangeKey);
	ChangeButton->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);

	UpdateTextIndex(NewTagIndex);
	UpdateKeyText(KeyText);
}

void USLKeyMappingWidget::UpdateTextIndex(const FName& NewTagIndex)
{
	TagIndex = NewTagIndex;
}

void USLKeyMappingWidget::UpdateTextFont(const FSlateFontInfo& FontInfo)
{
	FSlateFontInfo NewFontInfo;

	NewFontInfo = FontInfo;

	NewFontInfo.Size = TagText->GetFont().Size;
	TagText->SetFont(NewFontInfo);

	NewFontInfo.Size = KeyTextBox->GetFont().Size;
	KeyTextBox->SetFont(NewFontInfo);
}

void USLKeyMappingWidget::UpdateTagText(const FText& NewTagText)
{
	TagText->SetText(NewTagText);
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

const FName& USLKeyMappingWidget::GetTagIndex() const
{
	return TagIndex;
}

const EInputActionType USLKeyMappingWidget::GetActionType() const
{
	return ActionType;
}

void USLKeyMappingWidget::OnClickedChangeKey()
{
	USLSoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USLSoundSubsystem>();
	checkf(IsValid(SoundSubsystem), TEXT("Sound Subsystem is invalid"));
	SoundSubsystem->PlayUISound(ESLUISoundType::EUS_Click);

	KeyDelegate.Broadcast(ActionType);
}

void USLKeyMappingWidget::PlayHoverSound()
{
	USLSoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USLSoundSubsystem>();
	checkf(IsValid(SoundSubsystem), TEXT("Sound Subsystem is invalid"));
	SoundSubsystem->PlayUISound(ESLUISoundType::EUS_Hover);
}
