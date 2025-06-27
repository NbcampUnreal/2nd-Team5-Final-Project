// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLKeySettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLKeyMappingWidget.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"

void USLKeySettingWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	InitElementWidget();

	Super::InitWidget(NewUISubsystem);
}

void USLKeySettingWidget::OnUpdatedSettingValue()
{
	Super::OnUpdatedSettingValue();

	CheckValidOfUserDateSubsystem();
	const TMap<EInputActionType, FEnhancedActionKeyMapping> ActionKeyMap = UserDataSubsystem->GetActionKeyMap();

	for (USLKeyMappingWidget* MappingWidget : MappingWidgets)
	{
		EInputActionType InputActionType = MappingWidget->GetActionType();

		if (ActionKeyMap.Contains(InputActionType))
		{
			MappingWidget->UpdateKeyText(ActionKeyMap[InputActionType].Key.GetFName());
		}
	}
}

void USLKeySettingWidget::ApplyFontData()
{
	Super::ApplyFontData();

	for (USLKeyMappingWidget* MappingWidget : MappingWidgets)
	{
		MappingWidget->UpdateTextFont(FontInfo);
	}
}

void USLKeySettingWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FText> KeySettingTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_KeySetting)
		{
			KeySettingTextMap = UITextPool->TextMap;
			break;
		}
	}

	for (USLKeyMappingWidget* MappingWidget : MappingWidgets)
	{
		FName Index = MappingWidget->GetTagIndex();

		if (KeySettingTextMap.Contains(Index))
		{
			MappingWidget->UpdateTagText(KeySettingTextMap[Index]);
		}
	}
}

void USLKeySettingWidget::OnClickedKeyDataButton(EInputActionType TargetAction)
{
	TargetActionType = TargetAction;
	SetFocusToTargetButton(TargetAction, true);
	bOnClickedChange = true;
}

void USLKeySettingWidget::UpdateKeyMapping(const FKey& InputKey)
{
	CheckValidOfUserDateSubsystem();

	if (UserDataSubsystem->UpdateMappingKey(TargetActionType, InputKey))
	{
		for (USLKeyMappingWidget* MappingWidget : MappingWidgets)
		{
			if (MappingWidget->GetActionType() == TargetActionType)
			{
				MappingWidget->UpdateKeyText(InputKey.GetFName());
				break;
			}
		}
	}

	SetFocusToTargetButton(TargetActionType, false);
	TargetActionType = EInputActionType::EIAT_None;
}

void USLKeySettingWidget::SetFocusToTargetButton(EInputActionType TargetAction, bool bIsFocus)
{
	for (USLKeyMappingWidget* MappingWidget : MappingWidgets)
	{
		if (MappingWidget->GetActionType() == TargetAction)
		{
			MappingWidget->SetVisibilityButton(!bIsFocus);
			continue;
		}

		MappingWidget->SetVisibilityButton(true);
	}
}

void USLKeySettingWidget::CheckValidOfUserDateSubsystem()
{
	if (IsValid(UserDataSubsystem))
	{
		return;
	}

	UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();
	checkf(IsValid(UserDataSubsystem), TEXT("User Data Subsystem is invalid"));
}

FReply USLKeySettingWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bOnClickedChange)
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	bOnClickedChange = false;

	UpdateKeyMapping(InKeyEvent.GetKey());

	return FReply::Handled();
}

FReply USLKeySettingWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent)
{
	if (!bOnClickedChange)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InPointerEvent);
	}

	bOnClickedChange = false;

	UpdateKeyMapping(InPointerEvent.GetEffectingButton());

	return FReply::Handled();
}

void USLKeySettingWidget::InitElementWidget()
{
	CheckValidOfUserDateSubsystem();
	const TMap<EInputActionType, FEnhancedActionKeyMapping> ActionKeyMap = UserDataSubsystem->GetActionKeyMap();
	
	checkf(IsValid(KeyMappingWidgetClass), TEXT("Key Mapping Widget Class is invalid"));

	int32 GridIndex = 0;

	for (int32 i = (int32)EInputActionType::EIAT_None + 1; i < (int32)EInputActionType::EIAT_Max; ++i)
	{
		USLKeyMappingWidget* NewMappingWidget = CreateWidget<USLKeyMappingWidget>(this, KeyMappingWidgetClass);
		EInputActionType ActionType = (EInputActionType)i;

		if (!KeyTagIndexMap.Contains(ActionType) &&
			!ActionKeyMap.Contains(ActionType))
		{
			continue;
		}

		NewMappingWidget->InitWidget(ActionType, KeyTagIndexMap[ActionType], ActionKeyMap[ActionType].Key.GetFName());
		NewMappingWidget->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
		MappingWidgets.Add(NewMappingWidget);
		//ActionWidgetMap.Add(ActionType, NewMappingWidget);

		KeySettingGrid->AddChildToGrid(NewMappingWidget, GridIndex / 2, GridIndex % 2);
		++GridIndex;
	}
}
