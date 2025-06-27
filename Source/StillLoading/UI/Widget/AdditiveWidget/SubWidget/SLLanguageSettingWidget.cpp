// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLLanguageSettingWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"

const FName USLLanguageSettingWidget::LanguageTagIndex = "LanguageTag";

void USLLanguageSettingWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	Super::InitWidget(NewUISubsystem);

	LanguageLeftButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedLanguageLeftButton);
	LanguageRightButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedLanguageRightButton);

	LanguageLeftButton->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);
	LanguageRightButton->OnHovered.AddDynamic(this, &ThisClass::PlayHoverSound);
}

void USLLanguageSettingWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	OnUpdatedSettingValue();
}

void USLLanguageSettingWidget::OnUpdatedSettingValue()
{
	Super::OnUpdatedSettingValue();

	CheckValidOfUserDataSubsystem();

	ESLLanguageType CurrentLanguage = UserDataSubsystem->GetCurrentLanguage();

	if (LanguageTextMap.Contains(CurrentLanguage))
	{
		LanguageText->SetText(LanguageTextMap[CurrentLanguage]);
	}
}

void USLLanguageSettingWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FText> OptionTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Option)
		{
			OptionTextMap = UITextPool->TextMap;
			break;
		}
	}

	LanguageModeText->SetText(OptionTextMap[LanguageTagIndex]);
}

void USLLanguageSettingWidget::OnClickedLanguageLeftButton()
{
	UpdateLanguage(true);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLLanguageSettingWidget::OnClickedLanguageRightButton()
{
	UpdateLanguage(false);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLLanguageSettingWidget::UpdateLanguage(bool bIsLeft)
{
	CheckValidOfUserDataSubsystem();
	ESLLanguageType CurrentLanguage = UserDataSubsystem->GetCurrentLanguage();
	int32 NextLanguageIndex = 0;

	if (bIsLeft)
	{
		NextLanguageIndex = (int32)CurrentLanguage - 1;
	}
	else
	{
		NextLanguageIndex = (int32)CurrentLanguage + 1;
	}

	if (NextLanguageIndex == (int32)ESLLanguageType::EL_None)
	{
		NextLanguageIndex = (int32)ESLLanguageType::EL_Max - 1;
	}
	else if (NextLanguageIndex == (int32)ESLLanguageType::EL_Max)
	{
		NextLanguageIndex = (int32)ESLLanguageType::EL_None + 1;
	}

	CurrentLanguage = (ESLLanguageType)NextLanguageIndex;

	if (LanguageTextMap.Contains(CurrentLanguage))
	{
		UserDataSubsystem->SetLanguage(CurrentLanguage);
		LanguageText->SetText(LanguageTextMap[CurrentLanguage]);
	}
}

bool USLLanguageSettingWidget::ApplyListBackImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyListBackImage(SlateBrush))
	{
		return false;
	}

	LanguageBack->SetBrush(SlateBrush);
	return true;
}

bool USLLanguageSettingWidget::ApplyLeftArrowImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyLeftArrowImage(ButtonStyle))
	{
		return false;
	}

	LanguageLeftButton->SetStyle(ButtonStyle);
	return true;
}
bool USLLanguageSettingWidget::ApplyRightArrowImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyRightArrowImage(ButtonStyle))
	{
		return false;
	}

	LanguageRightButton->SetStyle(ButtonStyle);
	return true;
}

void USLLanguageSettingWidget::CheckValidOfUserDataSubsystem()
{
	if (IsValid(UserDataSubsystem))
	{
		return;
	}

	UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();
	checkf(IsValid(UserDataSubsystem), TEXT("User Data Subsystem is invalid"));
}