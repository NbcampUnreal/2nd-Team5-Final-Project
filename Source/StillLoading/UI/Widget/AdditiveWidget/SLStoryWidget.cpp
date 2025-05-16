// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLStoryWidget.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

const FName USLStoryWidget::StoryBackImgIndex = "StoryBackImg";
const FName USLStoryWidget::NameBackImgIndex = "NameBackImg";

void USLStoryWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_StoryWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 12;
	bIsVisibleCursor = true;

	ParentNamePanel = NamePanel;
	ParentNameText = NameText;
	ParentNextButton = NextButton;
	ParentTalkText = StoryText;

	Super::InitWidget(NewUISubsystem);
}

void USLStoryWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	UpdateStoryState(WidgetActivateBuffer.CurrentChapter, WidgetActivateBuffer.TargetStory, WidgetActivateBuffer.TargetIndex);
	Super::ActivateWidget(WidgetActivateBuffer);
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

	if (ImageMap.Contains(StoryBackImgIndex) &&
		IsValid(ImageMap[StoryBackImgIndex]))
		StoryBack->SetBrushFromTexture(ImageMap[StoryBackImgIndex]);

	if (ImageMap.Contains(NameBackImgIndex) &&
		IsValid(ImageMap[NameBackImgIndex]))
		NameBack->SetBrushFromTexture(ImageMap[NameBackImgIndex]);
}

void USLStoryWidget::ApplyFontData()
{
	Super::ApplyFontData();

	/*StoryText->SetFont(FontInfo);
	NameText->SetFont(FontInfo);*/
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