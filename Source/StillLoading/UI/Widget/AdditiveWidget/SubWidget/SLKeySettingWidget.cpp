// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLKeySettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLKeyMappingWidget.h"
#include "Components/Button.h"


void USLKeySettingWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::KeySettingWidget;

	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem, ChapterType);

	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);
	InitKeyDataMap();
}

void USLKeySettingWidget::ApplyImageData()
{

}

void USLKeySettingWidget::ApplyFontData()
{

}

void USLKeySettingWidget::OnClickedKeyDataButton(const FKey& TargetKey)
{
	// TODO : Update Agree Check
	TempKey = TargetKey;

	/*FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);*/

	SetFocusToTargetButton(TargetKey, true);

	bOnClickedChange = true;
}

void USLKeySettingWidget::UpdateKeyMapping(const FKey& InputKey)
{
	if (KeyDataMap.Contains(InputKey))
	{
		KeyDataMap[TempKey].ElementWidget->SetVisibilityButton(true);
		UE_LOG(LogTemp, Warning, TEXT("%s is Using Key"), *InputKey.ToString());
		return;
	}

	if (KeyDataMap.Contains(TempKey))
	{
		FSLKeySettingData TempData = KeyDataMap[TempKey];
		TempData.MappingData.Key = InputKey;
		TempData.ElementWidget->UpdateKeyText(InputKey);
		
		SetFocusToTargetButton(TempKey, false);

		KeyDataMap.Remove(TempKey);
		KeyDataMap.Add(InputKey, TempData);

		IMC->UnmapKey(TempData.MappingData.Action, TempKey);
		IMC->MapKey(TempData.MappingData.Action, InputKey);
	}
}

void USLKeySettingWidget::SetFocusToTargetButton(const FKey& TargetKey, bool bIsFocus)
{
	//KeyDataMap[TargetKey].ElementWidget->SetVisibilityButton(!bIsFocus);

	for (const TPair<FKey, FSLKeySettingData> KeyDataPair : KeyDataMap)
	{
		if (KeyDataPair.Key == TargetKey)
		{
			KeyDataPair.Value.ElementWidget->SetVisibilityButton(!bIsFocus);
			continue;
		}

		KeyDataPair.Value.ElementWidget->SetVisibilityButton(true);

		/*if (KeyDataPair.Key == TargetKey)
		{
			KeyDataPair.Value.ElementWidget->SetVisibilityButton(!bIsFocus);
			continue;
		}

		KeyDataPair.Value.ElementWidget->SetIsEnabledButton(!bIsFocus);*/
	}
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
	/*SetIsFocusable(true);
	SetKeyboardFocus();

	return FReply::Unhandled();*/
}

void USLKeySettingWidget::InitKeyDataMap()
{
	checkf(IsValid(IMC), TEXT("IMC is invalid"));

	TArray<FEnhancedActionKeyMapping> KeyMappingDatas = IMC->GetMappings();

	for (const FEnhancedActionKeyMapping& KeyMappingData : KeyMappingDatas)
	{
		AddToKeyDataMap(KeyMappingData);
	}
}

void USLKeySettingWidget::AddToKeyDataMap(const FEnhancedActionKeyMapping& TargetData)
{
	FSLKeySettingData KeySettingData;

	if (TargetData.Action->GetName().Contains(TEXT("MoveUp")))
	{
		KeySettingData.ActionName = "MoveUp";
		//KeySettingData.ActionType = EInputActionType::MoveUp;
		KeySettingData.ElementWidget = MoveUp;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("MoveDown")))
	{
		KeySettingData.ActionName = "MoveDown";
		//KeySettingData.ActionType = EInputActionType::MoveDown;
		KeySettingData.ElementWidget = MoveDown;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("MoveLeft")))
	{
		KeySettingData.ActionName = "MoveLeft";
		//KeySettingData.ActionType = EInputActionType::MoveLeft;
		KeySettingData.ElementWidget = MoveLeft;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("MoveRight")))
	{
		KeySettingData.ActionName = "MoveRight";
		//KeySettingData.ActionType = EInputActionType::MoveRight;
		KeySettingData.ElementWidget = MoveRight;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("Walk")))
	{
		KeySettingData.ActionName = "Walk";
		//KeySettingData.ActionType = EInputActionType::Walk;
		KeySettingData.ElementWidget = Walk;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("Jump")))
	{
		KeySettingData.ActionName = "Jump";
		//KeySettingData.ActionType = EInputActionType::Jump;
		KeySettingData.ElementWidget = Jump;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("Attack")))
	{
		KeySettingData.ActionName = "Attack";
		//KeySettingData.ActionType = EInputActionType::Attack;
		KeySettingData.ElementWidget = Attack;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("Interation")))
	{
		KeySettingData.ActionName = "Interaction";
		//KeySettingData.ActionType = EInputActionType::Interaction;
		KeySettingData.ElementWidget = Interaction;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("PointMove")))
	{
		KeySettingData.ActionName = "PointMove";
		//KeySettingData.ActionType = EInputActionType::PointMove;
		KeySettingData.ElementWidget = PointMove;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("Menu")))
	{
		KeySettingData.ActionName = "Menu";
		//KeySettingData.ActionType = EInputActionType::Menu;
		KeySettingData.ElementWidget = Menu;
	}
	else if (TargetData.Action->GetName().Contains(TEXT("Look")))
	{
		KeySettingData.ActionName = "Look";
		//KeySettingData.ActionType = EInputActionType::Look;
		KeySettingData.ElementWidget = Look;
	}

	KeySettingData.MappingData = TargetData;

	if (IsValid(KeySettingData.ElementWidget))
	{
		KeySettingData.ElementWidget->InitWidget(KeySettingData.ActionName, TargetData.Key);
		KeySettingData.ElementWidget->KeyDelegate.AddDynamic(this, &ThisClass::OnClickedKeyDataButton);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is invalid"), *TargetData.Action->GetName());
	}

	KeyDataMap.Add(TargetData.Key, KeySettingData);
}
