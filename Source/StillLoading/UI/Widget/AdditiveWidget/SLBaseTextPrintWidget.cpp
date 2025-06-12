// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"

void USLBaseTextPrintWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	Super::InitWidget(NewUISubsystem);

	if (IsValid(ParentNextButton))
	{
		ParentNextButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedNextButton);
	}

	if (IsValid(ParentNextButton))
	{
		ParentSkipButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedSkipButton);
	}

	if (IsValid(ParentNextButton))
	{
		ParentFastButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedFastButton);
	}
}

void USLBaseTextPrintWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	TargetTextIndex = 0;

	if (TalkArray.IsEmpty())
	{
		TalkDelegateBuffer.OnTalkEnded.Broadcast();
		CloseWidget();
		return;
	}

	ChangeTargetText();
}

FSLTalkDelegateBuffer& USLBaseTextPrintWidget::GetTalkDelegateBuffer()
{
	return TalkDelegateBuffer;
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
		TalkArray.Empty();
		TalkDelegateBuffer.OnTalkEnded.Broadcast();
		CloseWidget();
		return;
	}

	ChangeTargetText();
}

void USLBaseTextPrintWidget::OnClickedSkipButton()
{
	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);

	TalkArray.Empty();
	TalkDelegateBuffer.OnTalkEnded.Broadcast();
	CloseWidget();
}

void USLBaseTextPrintWidget::OnClickedFastButton()
{
	bIsFasted = !bIsFasted;
}

void USLBaseTextPrintWidget::PrintTalkText()
{
	if (CurrentTextIndex < 0)
	{
		return;
	}

	ParentTalkText->SetText(FText::FromString(TargetText.ToString().LeftChop(CurrentTextIndex)));
	--CurrentTextIndex;

	float NextPrintTime = PrintTime;

	if (bIsFasted && !FMath::IsNearlyZero(Accelerator))
	{
		NextPrintTime /= Accelerator;
	}

	GetWorld()->GetTimerManager().SetTimer(TextPrintTimer, this, &ThisClass::PrintTalkText, NextPrintTime, false);

	PlayUISound(ESLUISoundType::EUS_Talk);
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
