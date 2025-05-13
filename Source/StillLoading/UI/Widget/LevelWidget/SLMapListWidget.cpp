// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLMapListWidget.h"
#include "UI/Widget/LevelWidget/SubWidget/SLMapElementWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"

void USLMapListWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;

	Super::InitWidget(NewUISubsystem, ChapterType);

	for (int32 TypeNum = 1; TypeNum < (int32)ESLGameMapType::EGM_Max; ++TypeNum)
	{
		ESLGameMapType MapType = (ESLGameMapType)TypeNum;
		USLMapElementWidget* NewElement = CreateWidget<USLMapElementWidget>(this, MapElementWidgetClass);

		NewElement->InitMapElement(MapType);
		NewElement->OnClickedMapElement.AddDynamic(this, &ThisClass::OnClickedElement);
		GridPanel->AddChildToUniformGrid(NewElement, MapElementDataMap[MapType].TargetRow, MapElementDataMap[MapType].TargetCol);
		ElementMap.Add(MapType, NewElement);
	}
}

void USLMapListWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLMapListWidget::OnClickedElement(ESLGameMapType TargetMapType)
{
	//checkf(ElementMap.Contains(TargetMapType), TEXT("Target Map not contains TargetMapType"));
	//USLMapElementWidget* TargetElement = ElementMap[TargetMapType];

	checkf(MapElementDataMap.Contains(TargetMapType), TEXT("Element Data not contains TargetMapType"));
	UE_LOG(LogTemp, Warning, TEXT("On Clicked %s"), *MapElementDataMap[TargetMapType].MapName.ToString());
}

void USLMapListWidget::ApplyImageData()
{
	Super::ApplyImageData();
}

void USLMapListWidget::ApplyFontData()
{
	Super::ApplyFontData();
}
