// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLMapListWidget.h"
#include "UI/Widget/LevelWidget/SubWidget/SLMapElementWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "UI/Struct/SLLevelWidgetDataRow.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"

const FName USLMapListWidget::ListBackIndex = "MapListBack";

void USLMapListWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;

	Super::InitWidget(NewUISubsystem);
}

void USLMapListWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	if (!IsValid(WidgetActivateBuffer.LevelWidgetData))
	{
		return;
	}

	TArray<FSLMapListDataRow*> MapListDataArray;
	WidgetActivateBuffer.LevelWidgetData->GetAllRows(TEXT("Map List Context"), MapListDataArray);

	for (const FSLMapListDataRow* MapListData : MapListDataArray)
	{
		ESLGameMapType MapType = MapListData->GameMapType;
		USLMapElementWidget* NewElement = CreateWidget<USLMapElementWidget>(this, MapElementWidgetClass);

		NewElement->InitMapElement(MapType);
		NewElement->SetElementImgIndex(MapListData->ImageIndex);
		NewElement->OnClickedMapElement.AddDynamic(this, &ThisClass::OnClickedElement);
		GridPanel->AddChildToUniformGrid(NewElement, MapListData->TargetRow, MapListData->TargetCol);
		ElementMap.Add(MapType, NewElement);
	}

	Super::ActivateWidget(WidgetActivateBuffer);
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

	checkf(ElementMap.Contains(TargetMapType), TEXT("Element Data not contains TargetMapType"));
	UE_LOG(LogTemp, Warning, TEXT("On Clicked %s"), *ElementMap[TargetMapType]->GetName());

	UGameplayStatics::OpenLevel(GetWorld(), "TestInGameLevel");
}

void USLMapListWidget::ApplyImageData()
{
	Super::ApplyImageData();

	for (TPair<ESLGameMapType, USLMapElementWidget*>& Element : ElementMap)
	{
		FName Index = Element.Value->GetElementImgIndex();

		if (ImageMap.Contains(Index) &&
			IsValid(ImageMap[Index]))
		{
			Element.Value->SetMapElementImage(ImageMap[Index]);
		}
	}

	if (ImageMap.Contains(ListBackIndex) &&
		IsValid(ImageMap[ListBackIndex]))
	{
		BackgroundImg->SetBrushFromTexture(ImageMap[ListBackIndex]);
	}
}

void USLMapListWidget::ApplyFontData()
{
	Super::ApplyFontData();
}
