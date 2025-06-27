// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLBaseTextPrintWidget.h"
#include "UI/Widget/SLButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "SubSystem/SLUserDataSubsystem.h"

void USLBaseTextPrintWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	Super::InitWidget(NewUISubsystem);

	if (IsValid(ParentNextButton))
	{
		ParentNextButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedNextButton);
	}

	if (IsValid(ParentSkipButton))
	{
		ParentSkipButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedSkipButton);
		ParentSkipButton->SetButtonText(FText::FromString(FString::Printf(TEXT("SKIP"))));
		ParentSkipButton->InitButton();
	}

	if (IsValid(ParentFastButton))
	{
		ParentFastButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedFastButton);
		ParentFastButton->SetButtonText(FText::FromString(FString::Printf(TEXT(">>"))));
		ParentFastButton->InitButton();
	}

	if (IsValid(ParentAcceptButton))
	{
		ParentAcceptButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedAcceptButton);
		ParentAcceptButton->SetButtonText(FText::FromString(FString::Printf(TEXT("Yes"))));
		ParentAcceptButton->InitButton();
	}

	if (IsValid(ParentRejectButton))
	{
		ParentRejectButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedRejectButton);
		ParentRejectButton->SetButtonText(FText::FromString(FString::Printf(TEXT("No"))));
		ParentRejectButton->InitButton();
	}

	bIsFocusable = true;
}

void USLBaseTextPrintWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	TargetTextIndex = 0;

	if (TalkArray.IsEmpty())
	{
		OnTalkEnded.Broadcast();
		OnChoiceEnded.Broadcast(false);
		CloseWidget();
		return;
	}

	SetFocus();
	SetChoiceVisibility(false);
	ChangeTargetText();
}

void USLBaseTextPrintWidget::OnClickedNextButton()
{
	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);

	if (CurrentTextIndex >= 0)
	{
		CurrentTextIndex = -1;
		ParentTalkText->SetText(TalkArray[TargetTextIndex]);

		if (NameArray[TargetTextIndex] == "Choice")
		{
			SetChoiceVisibility(true);
		}

		return;
	}

	++TargetTextIndex;

	if (TargetTextIndex >= TalkArray.Num())
	{
		CloseTalk();
		OnChoiceEnded.Broadcast(false);

		return;
	}

	ChangeTargetText();
}

void USLBaseTextPrintWidget::OnClickedSkipButton()
{
	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);

	if (NameArray.Last() == "Choice")
	{
		TargetTextIndex = NameArray.Num() - 1;
		CurrentTextIndex = -1;
		ParentTalkText->SetText(TalkArray[TargetTextIndex]);
		SetChoiceVisibility(true);
	}
	else
	{
		CurrentTextIndex = -1;
		CloseTalk();
		OnChoiceEnded.Broadcast(false);
	}
}

void USLBaseTextPrintWidget::OnClickedFastButton()
{
	bIsFasted = !bIsFasted;

	if (IsValid(ParentFastButton))
	{
		if (bIsFasted)
		{
			ParentFastButton->SetButtonText(FText::FromString(FString::Printf(TEXT(">"))));
		}
		else
		{
			ParentFastButton->SetButtonText(FText::FromString(FString::Printf(TEXT(">>"))));
		}
	}
}

void USLBaseTextPrintWidget::OnClickedAcceptButton()
{
	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);

	if (CurrentTextIndex >= 0)
	{
		CurrentTextIndex = -1;
		ParentTalkText->SetText(TalkArray[TargetTextIndex]);
	}

	CloseTalk();
	OnChoiceEnded.Broadcast(true);
}

void USLBaseTextPrintWidget::OnClickedRejectButton()
{
	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);

	if (CurrentTextIndex >= 0)
	{
		CurrentTextIndex = -1;
		ParentTalkText->SetText(TalkArray[TargetTextIndex]);
	}

	CloseTalk();
	OnChoiceEnded.Broadcast(false);
}

FReply USLBaseTextPrintWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!IsInViewport())
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	FKey InputKey = InKeyEvent.GetKey();
	USLUserDataSubsystem* UserDataSubsystem = GetGameInstance()->GetSubsystem<USLUserDataSubsystem>();

	if (UserDataSubsystem->GetActionKeyMap().Contains(EInputActionType::EIAT_Interaction))
	{
		if (InputKey == UserDataSubsystem->GetActionKeyMap()[EInputActionType::EIAT_Interaction].Key)
		{
			OnClickedNextButton();
		}
	}

	return FReply::Handled();
}

void USLBaseTextPrintWidget::PrintTalkText()
{
	if (CurrentTextIndex < 0)
	{
		if (NameArray[TargetTextIndex] == "Choice")
		{
			SetChoiceVisibility(true);
		}

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

	TargetText = TalkArray[TargetTextIndex];
	CurrentTextIndex = TalkArray[TargetTextIndex].ToString().Len() - 1;

	FName TargetName = NameArray[TargetTextIndex];

	if (TargetName == "None" || TargetName == "Choice")
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

void USLBaseTextPrintWidget::SetChoiceVisibility(bool bIsVisible)
{
	if (bIsVisible)
	{
		ParentAcceptButton->SetVisibility(ESlateVisibility::Visible);
		ParentRejectButton->SetVisibility(ESlateVisibility::Visible);
		ParentFastButton->SetVisibility(ESlateVisibility::Collapsed);
		ParentSkipButton->SetVisibility(ESlateVisibility::Collapsed);
		ParentNextButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		ParentAcceptButton->SetVisibility(ESlateVisibility::Collapsed);
		ParentRejectButton->SetVisibility(ESlateVisibility::Collapsed);
		ParentFastButton->SetVisibility(ESlateVisibility::Visible);
		ParentSkipButton->SetVisibility(ESlateVisibility::Visible);
		ParentNextButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void USLBaseTextPrintWidget::CloseTalk()
{
	TalkArray.Empty();
	OnTalkEnded.Broadcast();
	CloseWidget();
}
