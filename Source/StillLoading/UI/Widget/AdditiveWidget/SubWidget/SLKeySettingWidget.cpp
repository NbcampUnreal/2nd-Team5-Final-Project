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

const FName USLKeySettingWidget::SettingBackIndex = "KeySettingBack";

void USLKeySettingWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_KeySettingWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 17;
	bIsVisibleCursor = true;

	InitElementWidget();
	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem);

	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);
}

void USLKeySettingWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	if (IsValid(OpenAnim))
	{
		PlayAnimation(OpenAnim);
	}
	else
	{
		OnEndedOpenAnim();
	}

	PlayUISound(ESLUISoundType::EUS_Open);
}

void USLKeySettingWidget::DeactivateWidget()
{
	if (IsValid(CloseAnim))
	{
		PlayAnimation(CloseAnim);
	}
	else
	{
		OnEndedCloseAnim();
	}

	PlayUISound(ESLUISoundType::EUS_Close);
}

void USLKeySettingWidget::ApplyImageData()
{
	Super::ApplyImageData();

	if (ImageMap.Contains(SettingBackIndex) &&
		IsValid(ImageMap[SettingBackIndex]))
	{
		BackgroundImg->SetBrushFromTexture(ImageMap[SettingBackIndex]);
	}
}

void USLKeySettingWidget::ApplyFontData()
{
	Super::ApplyFontData();

	/*for (const TPair<EInputActionType, USLKeyMappingWidget*>& KeyElement : ActionWidgetMap)
	{
		KeyElement.Value->UpdateTextFont(FontInfo);
	}*/
}

void USLKeySettingWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FSLUITextData> KeySettingTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_KeySetting)
		{
			KeySettingTextMap = UITextPool->TextMap;
			break;
		}
	}

	for (const TPair<EInputActionType, USLKeyMappingWidget*> ActionPair : ActionWidgetMap)
	{
		FName Index = ActionPair.Value->GetTagIndex();

		if (KeySettingTextMap.Contains(Index) &&
			KeySettingTextMap[Index].ChapterTextMap.Contains(ESLChapterType::EC_Intro))
		{
			ActionPair.Value->UpdateTagText(KeySettingTextMap[Index].ChapterTextMap[ESLChapterType::EC_Intro]);
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
		if (ActionWidgetMap.Contains(TargetActionType))
		{
			ActionWidgetMap[TargetActionType]->UpdateKeyText(InputKey.GetFName());
		}
	}

	SetFocusToTargetButton(TargetActionType, false);
	TargetActionType = EInputActionType::EIAT_None;
}

void USLKeySettingWidget::SetFocusToTargetButton(EInputActionType TargetAction, bool bIsFocus)
{
	for (const TPair<EInputActionType, USLKeyMappingWidget*> KeyDataPair : ActionWidgetMap)
	{
		if (KeyDataPair.Key == TargetAction)
		{
			KeyDataPair.Value->SetVisibilityButton(!bIsFocus);
			continue;
		}

		KeyDataPair.Value->SetVisibilityButton(true);
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
	//int32 ElementSize = ActionKeyMap.Num();

	for (int32 i = 1; i <= (int32)EInputActionType::EIAT_Menu; ++i)
	{
		USLKeyMappingWidget* NewMappingWidget = CreateWidget<USLKeyMappingWidget>(this, KeyMappingWidgetClass);
		EInputActionType ActionType = (EInputActionType)i;

		NewMappingWidget->InitWidget(ActionType, KeyTagIndexMap[ActionType], ActionKeyMap[ActionType].Key.GetFName());
		NewMappingWidget->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
		ActionWidgetMap.Add(ActionType, NewMappingWidget);

		//KeySettingGrid->AddChildToGrid(NewMappingWidget, GridIndex % / (ElementSize / 2), GridIndex / (ElementSize / 2));
		KeySettingGrid->AddChildToGrid(NewMappingWidget, GridIndex / 2, GridIndex % 2);
		++GridIndex;
	}
}
