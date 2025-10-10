// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLSaveSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetTree.h"

void USLSaveSlotWidget::InitWidget(int32 Number)
{
	SlotNum = Number;
	SlotNumber->SetText(FText::FromString(FString::FromInt(SlotNum)));
	SlotButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedSlot);
}

void USLSaveSlotWidget::UpdateTextFont(const FSlateFontInfo& FontInfo, float FontOffset)
{
	TArray<UWidget*> FoundWidgets;
	WidgetTree->GetAllWidgets(FoundWidgets);

	FSlateFontInfo NewFontInfo;
	NewFontInfo = FontInfo;

	for (UWidget* Widget : FoundWidgets)
	{
		if (IsValid(Widget))
		{
			UTextBlock* TextBlock = Cast<UTextBlock>(Widget);

			if (IsValid(TextBlock))
			{
				NewFontInfo.Size = TextBlock->GetFont().Size;
				TextBlock->SetFont(NewFontInfo);
				TextBlock->SetRenderTranslation(FVector2D(0, FontOffset));
			}
		}
	}
}

void USLSaveSlotWidget::SetEmptyMode(bool bIsEmpty, bool bIsLoad)
{
	if (bIsEmpty)
	{
		EmptyCover->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (bIsLoad)
		{
			SlotButton->SetIsEnabled(false);
		}
		else
		{
			SlotButton->SetIsEnabled(true);
		}
	}
	else
	{
		EmptyCover->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USLSaveSlotWidget::SetSlotImage(FSlateBrush& SlateBrush)
{
	SlotImg->SetBrush(SlateBrush);
	EmptyImg->SetBrush(SlateBrush);
}

void USLSaveSlotWidget::SetMapImage(UObject* ImageObj)
{
	if (IsValid(ImageObj))
	{
		FSlateBrush SlateBrush;
		SlateBrush.SetResourceObject(ImageObj);
		MapImg->SetBrush(SlateBrush);
	}
}

void USLSaveSlotWidget::SetChapterNumber(const FText& Chapter)
{
	ChapterNumber->SetText(Chapter);
}

void USLSaveSlotWidget::SetMapName(const FText& Name)
{
	MapName->SetText(Name);
}

void USLSaveSlotWidget::SetPlayTime(const float TimeValue)
{
	int32 Seconds = (int32)TimeValue % 60;
	int32 Mins = (int32)(TimeValue / 60);
	int32 Hours = Mins / 60;
	Mins %= 60;

	PlayTime->SetText(FText::FromString(FString::Printf(TEXT("%02d : %02d : %02d"), Hours, Mins, Seconds)));
}

void USLSaveSlotWidget::SetSaveTime(const FDateTime& TimeValue)
{
	int32 Year = TimeValue.GetYear();
	int32 Month = TimeValue.GetMonth();
	int32 Day = TimeValue.GetDay();
	int32 Hours = TimeValue.GetHour();
	int32 Mins = TimeValue.GetMinute();

	SaveTime->SetText(FText::FromString(FString::Printf(TEXT("%d.%02d.%02d _ %02d : %02d"), Year, Month, Day, Hours, Mins)));
}

void USLSaveSlotWidget::OnClickedSlot()
{
	OnClicked.Broadcast(SlotNum);
}
