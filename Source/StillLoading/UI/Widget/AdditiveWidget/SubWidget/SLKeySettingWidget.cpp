// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLKeySettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLKeyMappingWidget.h"
#include "SubSystem/SLUserDataSubsystem.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"

const FName USLKeySettingWidget::MoveUpTagIndex = "MoveUp";
const FName USLKeySettingWidget::MoveDownTagIndex = "MoveDown";
const FName USLKeySettingWidget::MoveLeftTagIndex = "MoveLeft";
const FName USLKeySettingWidget::MoveRightTagIndex = "MoveRight";
const FName USLKeySettingWidget::WalkTagIndex = "Walk";
const FName USLKeySettingWidget::JumpTagIndex = "Jump";
const FName USLKeySettingWidget::AttackTagIndex = "Attack";
const FName USLKeySettingWidget::InteractionTagIndex = "Interaction";
const FName USLKeySettingWidget::PointMoveTagIndex = "PointMove";
const FName USLKeySettingWidget::LookTagIndex = "Look";
const FName USLKeySettingWidget::MenuTagIndex = "Menu";

void USLKeySettingWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::EAW_KeySettingWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 17;
	bIsVisibleCursor = true;
	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem, ChapterType);

	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);

	InitElementWidget();
}

void USLKeySettingWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

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

}

void USLKeySettingWidget::ApplyFontData()
{

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

	UE_LOG(LogTemp, Warning, TEXT("Input Key is %s"), *InKeyEvent.GetKey().ToString());

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

	UE_LOG(LogTemp, Warning, TEXT("Mouse Button Down. Key Name : %s"), *InPointerEvent.GetEffectingButton().ToString());
	
	bOnClickedChange = false;

	UpdateKeyMapping(InPointerEvent.GetEffectingButton());

	return FReply::Handled();
}

void USLKeySettingWidget::InitElementWidget()
{
	CheckValidOfUserDateSubsystem();
	const TMap<EInputActionType, FEnhancedActionKeyMapping> ActionKeyMap = UserDataSubsystem->GetActionKeyMap();
	EInputActionType TargetType = EInputActionType::EIAT_None;

	// TODO : Apply TextPool To Init Widget Tag Text

	TargetType = EInputActionType::EIAT_MoveUp;
	MoveUp->InitWidget(MoveUpTagIndex, TargetType);
	MoveUp->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	MoveUp->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, MoveUp);

	TargetType = EInputActionType::EIAT_MoveDown;
	MoveDown->InitWidget(MoveDownTagIndex, TargetType);
	MoveDown->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	MoveDown->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, MoveDown);

	TargetType = EInputActionType::EIAT_MoveLeft;
	MoveLeft->InitWidget(MoveLeftTagIndex, TargetType);
	MoveLeft->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	MoveLeft->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, MoveLeft);

	TargetType = EInputActionType::EIAT_MoveRight;
	MoveRight->InitWidget(MoveRightTagIndex, TargetType);
	MoveRight->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	MoveRight->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, MoveRight);

	TargetType = EInputActionType::EIAT_Walk;
	Walk->InitWidget(WalkTagIndex, TargetType);
	Walk->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	Walk->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, Walk);

	TargetType = EInputActionType::EIAT_Jump;
	Jump->InitWidget(JumpTagIndex, TargetType);
	Jump->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	Jump->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, Jump);

	TargetType = EInputActionType::EIAT_Attack;
	Attack->InitWidget(AttackTagIndex, TargetType);
	Attack->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	Attack->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, Attack);

	TargetType = EInputActionType::EIAT_Interaction;
	Interaction->InitWidget(InteractionTagIndex, TargetType);
	Interaction->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	Interaction->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, Interaction);

	TargetType = EInputActionType::EIAT_PointMove;
	PointMove->InitWidget(PointMoveTagIndex, TargetType);
	PointMove->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	PointMove->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, PointMove);

	TargetType = EInputActionType::EIAT_Look;
	Look->InitWidget(LookTagIndex, TargetType);
	Look->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	Look->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, Look);

	TargetType = EInputActionType::EIAT_Menu;
	Menu->InitWidget(MenuTagIndex, TargetType);
	Menu->UpdateKeyText(ActionKeyMap[TargetType].Key.GetFName());
	Menu->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	ActionWidgetMap.Add(TargetType, Menu);
}
