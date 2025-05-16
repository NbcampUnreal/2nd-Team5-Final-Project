// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "UI/SLUISubsystem.h"
#include "UI/Struct/SLWidgetImageDataRow.h"
#include "Engine/Font.h"

void USLAdditiveWidget::FindWidgetData()
{
	CheckValidOfUISubsystem();

	const UDataTable* ImageDataTable = UISubsystem->GetImageDataTable();
	const FString ContextString(TEXT("Image Data Table"));

	TArray<FSLWidgetImageDataRow*> ImageData;

	ImageDataTable->GetAllRows(ContextString, ImageData);
	ImageMap.Empty();

	for (const FSLWidgetImageDataRow* ImageDataRow : ImageData)
	{
		if (ImageDataRow->WidgetType == WidgetType &&
			ImageDataRow->TargetChapter == CurrentChapter)
		{
			FontInfo = ImageDataRow->FontInfo;

			for (const TPair<FName, TSoftObjectPtr<UTexture2D>>& Target : ImageDataRow->ImageMap)
			{
				ImageMap.Add(Target.Key, Target.Value.LoadSynchronous());
			}

			break;
		}
	}
}

ESLAdditiveWidgetType USLAdditiveWidget::GetWidgetType() const
{
	return WidgetType;
}

void USLAdditiveWidget::CloseWidget()
{
	CheckValidOfUISubsystem();
	UISubsystem->RemoveCurrentAdditiveWidget(WidgetType);
}
