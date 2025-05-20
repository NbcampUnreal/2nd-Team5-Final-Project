// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLStoryWidget.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

void USLStoryWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_StoryWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 12;
	bIsVisibleCursor = true;

	ParentNamePanel = NamePanel;
	ParentNameText = NameText;
	ParentNextButton = NextButton;
	ParentSkipButton = SkipButton;
	ParentFastButton = FastButton;
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

bool USLStoryWidget::ApplyTextBorderImage()
{
	if (!Super::ApplyTextBorderImage())
	{
		return false;
	}

	StoryBack->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_TextBorder]);
	NameBack->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_TextBorder]);
	return true;
}

bool USLStoryWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyButtonImage(ButtonStyle))
	{
		return false;
	}

	SkipButton->SetStyle(ButtonStyle);
	FastButton->SetStyle(ButtonStyle);

	return true;
}
