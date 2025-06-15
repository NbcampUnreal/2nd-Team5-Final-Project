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

	//ParentNamePanel = NamePanel;
	//ParentNameText = NameText;
	//ParentNextButton = NextButton;
	//ParentSkipButton = SkipButton;
	//ParentFastButton = FastButton;
	//ParentTalkText = StoryText;

	Super::InitWidget(NewUISubsystem);
}

void USLStoryWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	UpdateStoryState(WidgetActivateBuffer.TargetStory, WidgetActivateBuffer.TalkName);
	Super::ActivateWidget(WidgetActivateBuffer);
}

void USLStoryWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	TalkArray.Empty();
	NameArray.Empty();
	OnEndedCloseAnim();
}

void USLStoryWidget::UpdateStoryState(ESLStoryType TargetStoryType, const FName& StoryName)
{
	CheckValidOfTextPoolSubsystem();

	const UDataTable* StoryDataTable = TextPoolSubsystem->GetStoryTextPool();

	TArray<FSLStoryTextPoolDataRow*> StoryDataArray;
	StoryDataTable->GetAllRows(TEXT("Story Text Context"), StoryDataArray);

	for (const FSLStoryTextPoolDataRow* TalkData : StoryDataArray)
	{
		if (TalkData->TextMap.Contains(TargetStoryType))
		{
			TalkArray = TalkData->TextMap[TargetStoryType].TalkTextArray;
			NameArray = TalkData->TextMap[TargetStoryType].TalkOwnArray;
			CurrentStoryType = TargetStoryType;
			CurrentStoryName = StoryName;
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
	UpdateStoryState(CurrentStoryType, CurrentStoryName);
	ChangeTargetText();
	PrintTalkText();
}

bool USLStoryWidget::ApplyTextBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyTextBorderImage(SlateBrush))
	{
		return false;
	}

	StoryBack->SetBrush(SlateBrush);

	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_NameBorder) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_NameBorder]))
	{
		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_NameBorder]);
		NameBack->SetBrush(SlateBrush);
	}

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
