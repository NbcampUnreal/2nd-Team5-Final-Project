// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

void USLBaseTextPrintWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	Super::InitWidget(NewUISubsystem, ChapterType);

	ParentNextButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedNextButton);
}

void USLBaseTextPrintWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	TargetTextIndex = 0;
	ChangeTargetText();
}

void USLBaseTextPrintWidget::OnClickedNextButton()
{
	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);

	if (CurrentTextIndex >= 0)
	{
		CurrentTextIndex = -1;
		ParentTalkText->SetText(TalkArray[TargetTextIndex]);
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

void USLBaseTextPrintWidget::PrintTalkText()
{
	if (CurrentTextIndex < 0)
	{
		return;
	}

	ParentTalkText->SetText(FText::FromString(TargetText.ToString().LeftChop(CurrentTextIndex)));
	--CurrentTextIndex;

	GetWorld()->GetTimerManager().SetTimer(TextPrintTimer, this, &ThisClass::PrintTalkText, PrintTime, false);
}

void USLBaseTextPrintWidget::ChangeTargetText()
{
	ParentTalkText->SetText(FText::GetEmpty());

	TargetText = TalkArray[TargetTextIndex];;
	CurrentTextIndex = TalkArray[TargetTextIndex].ToString().Len() - 1;

	FName TargetName = NameArray[TargetTextIndex];

	if (TargetName == "None")
	{
		ParentNamePanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		ParentNameText->SetText(FText::FromName(TargetName));
		ParentNamePanel->SetVisibility(ESlateVisibility::Visible);
	}

	PrintTalkText();
}
