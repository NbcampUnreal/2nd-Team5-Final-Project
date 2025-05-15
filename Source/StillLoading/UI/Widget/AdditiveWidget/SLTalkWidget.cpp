// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLTalkWidget.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

void USLTalkWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::EAW_TalkWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;

	Super::InitWidget(NewUISubsystem, ChapterType);

	NextButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedNextButton);
}

void USLTalkWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	TargetTextIndex = 0;
	ChangeTargetText();
}

void USLTalkWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLTalkWidget::UpdateTalkState(ESLTalkTargetType TalkTargetType, int32 TargetIndex)
{
	CheckValidOfTextPoolSubsystem();

	const UDataTable* TalkDataTable = TextPoolSubsystem->GetTalkTextPool();

	TArray<FSLTalkTextPoolDataRow*> TalkDataArray; 
	TalkDataTable->GetAllRows(TEXT("Talk Text Context"), TalkDataArray);

	for (const FSLTalkTextPoolDataRow* TalkData : TalkDataArray)
	{
		if (TalkData->TalkTarget == TalkTargetType &&
			TalkData->TalkMap.Contains(TargetIndex))
		{
			TalkArray = TalkData->TalkMap[TargetIndex].TalkTextArray;
			NameArray = TalkData->TalkMap[TargetIndex].TalkOwnArray;
			CurrentTalkType = TalkTargetType;
			CurrentTalkIndex = TargetIndex;
			break;
		}
	}
}

void USLTalkWidget::ApplyImageData()
{
	Super::ApplyImageData();
}

void USLTalkWidget::ApplyFontData()
{
	Super::ApplyFontData();
}

void USLTalkWidget::ApplyTextData()
{
	Super::ApplyTextData();

	if (!this->IsInViewport())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);
	UpdateTalkState(CurrentTalkType, CurrentTalkIndex);
	ChangeTargetText();
	PrintTalkText();
}

void USLTalkWidget::OnClickedNextButton()
{
	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);

	if (CurrentTextIndex >= 0)
	{
		CurrentTextIndex = -1;
		TalkText->SetText(TalkArray[TargetTextIndex]);
		return;
	}

	++TargetTextIndex;

	if (TargetTextIndex >= TalkArray.Num())
	{
		CloseWidget();
		return;
	}

	ChangeTargetText();
}

void USLTalkWidget::PrintTalkText()
{
	if (CurrentTextIndex < 0)
	{
		return;
	}

	TalkText->SetText(FText::FromString(TargetText.ToString().LeftChop(CurrentTextIndex)));
	--CurrentTextIndex;

	GetWorld()->GetTimerManager().SetTimer(TextPrintTimer, this, &ThisClass::PrintTalkText, PrintTime, false);
}

void USLTalkWidget::ChangeTargetText()
{
	TalkText->SetText(FText::GetEmpty());

	TargetText = TalkArray[TargetTextIndex];
	CurrentTextIndex = TalkArray[TargetTextIndex].ToString().Len() - 1;

	FName TargetName = NameArray[TargetTextIndex];

	if (TargetName == "None")
	{
		NamePanel->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	NameText->SetText(FText::FromName(TargetName));
	NamePanel->SetVisibility(ESlateVisibility::Visible);

	PrintTalkText();
}
