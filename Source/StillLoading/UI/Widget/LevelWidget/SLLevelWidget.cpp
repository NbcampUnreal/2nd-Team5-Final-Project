// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "UI/SLUISubsystem.h"
#include "UI/Struct/SLLevelWidgetDataRow.h"

void USLLevelWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetOrder = 0;
	Super::InitWidget(NewUISubsystem);
}

void USLLevelWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	CheckValidOfUISubsystem();
	UISubsystem->SetLevelInputMode(WidgetInputMode, bIsVisibleCursor);

	Super::ActivateWidget(WidgetActivateBuffer);
}

void USLLevelWidget::RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType TargetWidgetType)
{
	CheckValidOfUISubsystem();
	UISubsystem->AddAdditveWidget(TargetWidgetType);
}

void USLLevelWidget::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::FindWidgetData(WidgetActivateBuffer);

	if (WidgetActivateBuffer.WidgetImageData == nullptr)
	{
		return;
	}

	const UDataTable* ImageDataTable = WidgetActivateBuffer.WidgetImageData;
	const FString ContextString(TEXT("Image Data Table"));

	TArray<FSLLevelWidgetDataRow*> ImageData;

	ImageDataTable->GetAllRows(ContextString, ImageData);
	ImageMap.Empty();

	for (const FSLLevelWidgetDataRow* WidgetImgData : ImageData)
	{
		if (WidgetImgData->TargetChapter == WidgetActivateBuffer.CurrentChapter)
		{
			for (const TPair<FName, TSoftObjectPtr<UTexture2D>>& ImagePair : WidgetImgData->ImageMap)
			{
				ImageMap.Add(ImagePair.Key, ImagePair.Value.LoadSynchronous());
			}

			FontInfo = WidgetImgData->FontInfo;

			break;
		}
	}
}
