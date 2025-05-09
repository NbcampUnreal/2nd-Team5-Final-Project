// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLKeySettingWidget.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLKeyMappingWidget.h"


void USLKeySettingWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::KeySettingWidget;

	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem, ChapterType);

	InitKeyInfoWidgetMap();
}

void USLKeySettingWidget::ApplyImageData()
{

}

void USLKeySettingWidget::ApplyFontData()
{

}

void USLKeySettingWidget::ChangeKeyData(const FName& TargetName)
{
	
}

void USLKeySettingWidget::InitKeyDataMap()
{
	checkf(IsValid(IMC), TEXT("IMC is invalid"));

	TArray<FEnhancedActionKeyMapping> KeyMappingDatas = IMC->GetMappings();

	for (const FEnhancedActionKeyMapping& KeyMappingData : KeyMappingDatas)
	{
		AddToKeyDataMap(KeyMappingData.Key, KeyMappingData.Action);
		//AddIMCKeyMap(KeyMappingData.Action->GetName(), KeyMappingData.Key);
	}
}

void USLKeySettingWidget::AddToKeyDataMap(const FKey& NewKey, const UInputAction* NewInputAction)
{

}

void USLKeySettingWidget::InitKeyInfoWidgetMap()
{
	GetIMCKey();

	EInputActionType TargetType = EInputActionType::MoveUp;
	FName KeyName = (FName)IMCKeyMap[TargetType].ToString();

	KeyInfoWidgetMap.Add(KeyName, MoveUp);
	MoveUp->InitWidget("이동(전)", KeyName);
	
	TargetType = EInputActionType::MoveDown;
	KeyName = (FName)IMCKeyMap[TargetType].ToString();
	KeyInfoWidgetMap.Add(KeyName, MoveDown);
	MoveDown->InitWidget("이동(후)", KeyName);

	TargetType = EInputActionType::MoveLeft;
	KeyName = (FName)IMCKeyMap[TargetType].ToString();
	KeyInfoWidgetMap.Add(KeyName, MoveLeft);
	MoveLeft->InitWidget("이동(좌)", KeyName);

	TargetType = EInputActionType::MoveRight;
	KeyName = (FName)IMCKeyMap[TargetType].ToString();
	KeyInfoWidgetMap.Add(KeyName, MoveRight);
	MoveRight->InitWidget("이동(우)", KeyName);
}

void USLKeySettingWidget::GetIMCKey()
{
	checkf(IsValid(IMC), TEXT("IMC is invalid"));

	TArray<FEnhancedActionKeyMapping> KeyMappingDatas = IMC->GetMappings();

	for (const FEnhancedActionKeyMapping& KeyMappingData : KeyMappingDatas)
	{
		AddIMCKeyMap(KeyMappingData.Action->GetName(), KeyMappingData.Key);
	}
}

void USLKeySettingWidget::AddIMCKeyMap(const FString& ActionName, const FKey& Key)
{
	if (ActionName.Contains(TEXT("MoveUp")))
	{
		IMCKeyMap.Add(EInputActionType::MoveUp, Key);
	}
	else if (ActionName.Contains(TEXT("MoveDown")))
	{
		IMCKeyMap.Add(EInputActionType::MoveDown, Key);
	}
	else if (ActionName.Contains(TEXT("MoveLeft")))
	{
		IMCKeyMap.Add(EInputActionType::MoveLeft, Key);
	}
	else if (ActionName.Contains(TEXT("MoveRight")))
	{
		IMCKeyMap.Add(EInputActionType::MoveRight, Key);
	}
	else if (ActionName.Contains(TEXT("Walk")))
	{
		IMCKeyMap.Add(EInputActionType::Walk, Key);
	}
	else if (ActionName.Contains(TEXT("Jump")))
	{
		IMCKeyMap.Add(EInputActionType::Jump, Key);
	}
	else if (ActionName.Contains(TEXT("Attack")))
	{
		IMCKeyMap.Add(EInputActionType::Attack, Key);
	}
	else if (ActionName.Contains(TEXT("Interaction")))
	{
		IMCKeyMap.Add(EInputActionType::Interaction, Key);
	}
	else if (ActionName.Contains(TEXT("PointMove")))
	{
		IMCKeyMap.Add(EInputActionType::PointMove, Key);
	}
	else if (ActionName.Contains(TEXT("Menu")))
	{
		IMCKeyMap.Add(EInputActionType::Menu, Key);
	}
	else if (ActionName.Contains(TEXT("Look")))
	{
		IMCKeyMap.Add(EInputActionType::Look, Key);
	}
}
