// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLStoryWidget.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

void USLStoryWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::EAW_StoryWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;

	ParentNamePanel = NamePanel;
	ParentNameText = NameText;
	ParentNextButton = NextButton;
	ParentTalkText = StoryText;

	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLStoryWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

}

void USLStoryWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	TalkArray.Empty();
	NameArray.Empty();
	OnEndedCloseAnim();
}

void USLStoryWidget::UpdateStoryState(ESLChapterType ChapterType, ESLStoryType TargetStoryType, int32 TargetIndex)
{
	CheckValidOfTextPoolSubsystem();

	const UDataTable* StoryDataTable = TextPoolSubsystem->GetStoryTextPool();

	TArray<FSLStoryTextPoolDataRow*> StoryDataArray;
	StoryDataTable->GetAllRows(TEXT("Story Text Context"), StoryDataArray);

	for (const FSLStoryTextPoolDataRow* TalkData : StoryDataArray)
	{
		if (TalkData->Chapter == ChapterType &&
			TalkData->TextMap.Contains(TargetStoryType))
		{
			TalkArray = TalkData->TextMap[TargetStoryType].TalkTextArray;
			NameArray = TalkData->TextMap[TargetStoryType].TalkOwnArray;
			CurrentStoryType = TargetStoryType;
			CurrentStoryIndex = TargetIndex;
			break;
		}
	}
}

void USLStoryWidget::ApplyImageData()
{
	Super::ApplyImageData();
}

void USLStoryWidget::ApplyFontData()
{
	Super::ApplyFontData();
}

void USLStoryWidget::ApplyTextData()
{
	Super::ApplyTextData();

	if (!this->IsInViewport())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);
	UpdateStoryState(CurrentChapter, CurrentStoryType, CurrentStoryIndex);
	ChangeTargetText();
	PrintTalkText();
}