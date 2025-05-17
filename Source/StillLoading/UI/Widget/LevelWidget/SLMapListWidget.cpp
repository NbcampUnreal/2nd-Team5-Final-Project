// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLMapListWidget.h"
#include "UI/Widget/LevelWidget/SubWidget/SLMapElementWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "UI/Struct/SLLevelWidgetDataRow.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "UI/Widget/SLWidgetPrivateDataAsset.h"

void USLMapListWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;

	Super::InitWidget(NewUISubsystem);
}

void USLMapListWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	if (IsValid(WidgetActivateBuffer.WidgetPrivateData))
	{
		USLMapListPrivateDataAsset* PrivateData = Cast<USLMapListPrivateDataAsset>(WidgetActivateBuffer.WidgetPrivateData);
		TMap<ESLGameMapType, UTexture2D*> MapImages = PrivateData->GetMapImageByChapter(WidgetActivateBuffer.CurrentChapter).MapImageMap;
		TMap<ESLGameMapType, FSLMapListDataRow> MapDatas = PrivateData->GetMapDataByChapter(WidgetActivateBuffer.CurrentChapter).DataMap;

		for (const TPair<ESLGameMapType, FSLMapListDataRow>& MapData : MapDatas)
		{
			USLMapElementWidget* NewElement = CreateWidget<USLMapElementWidget>(this, MapElementWidgetClass);

			NewElement->InitMapElement(MapData.Key);
			NewElement->OnClickedMapElement.AddDynamic(this, &ThisClass::OnClickedElement);
			//NewElement->SetIsEnabledButton(MapData.Value.DefaultEnabled);
			NewElement->SetIsEnabled(MapData.Value.DefaultEnabled);

			if (MapImages.Contains(MapData.Key))
			{
				NewElement->SetMapElementImage(MapImages[MapData.Key]);
			}

			GridPanel->AddChildToUniformGrid(NewElement, MapData.Value.TargetRow, MapData.Value.TargetCol);
			ElementMap.Add(MapData.Key, NewElement);
		}
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

void USLMapListWidget::ApplyFontData()
{
	Super::ApplyFontData();
}

bool USLMapListWidget::ApplyBackgroundImage()
{
	if (!Super::ApplyBorderImage())
	{
		return false;
	}

	BackgroundImg->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_Background]);
	return true;
}

bool USLMapListWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();
	/*for (TPair<ESLGameMapType, USLMapElementWidget*>& Element : ElementMap)
	{
		FName Index = Element.Value->GetElementImgIndex();

		if (ImageMap.Contains(Index) &&
			IsValid(ImageMap[Index]))
		{
			Element.Value->SetMapElementImage(ImageMap[Index]);
		}
	}*/
	return false;
}
