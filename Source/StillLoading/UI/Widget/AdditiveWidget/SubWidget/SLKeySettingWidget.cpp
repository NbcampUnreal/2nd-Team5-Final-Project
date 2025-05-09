// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLKeySettingWidget.h"



void USLKeySettingWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::KeySettingWidget;

	// TODO : Bind OpenAnimation To OpenAnim, CloseAnimation To CloseAnim
	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLKeySettingWidget::ApplyImageData()
{

}

void USLKeySettingWidget::ApplyFontData()
{

}

void USLKeySettingWidget::ChangeKeyData()
{
	/*UInputMappingContext* IMC = NewObject<UInputMappingContext>();
	TArray<FEnhancedActionKeyMapping> KeyMapping = IMC->GetMappings();
	
	for (const FEnhancedActionKeyMapping& InputKey : KeyMapping)
	{
		InputKey.Key;
	}*/
}

void USLKeySettingWidget::GetIMCKey()
{
	if (!IsValid(IMC))
	{
		return;
	}

	TArray<FEnhancedActionKeyMapping> KeyMappingDatas = IMC->GetMappings();

	for (const FEnhancedActionKeyMapping& KeyMappingData : KeyMappingDatas)
	{
		FKey NewKey = KeyMappingData.Key;
		const UInputAction* NewIA = KeyMappingData.Action;
		//IMCKeyMap.Add();
	}
}
