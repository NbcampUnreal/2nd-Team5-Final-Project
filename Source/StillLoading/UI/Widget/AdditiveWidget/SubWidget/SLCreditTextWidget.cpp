// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLCreditTextWidget.h"
#include "Components/TextBlock.h"

void USLCreditTextWidget::SetText(const FText& TargetText)
{
	CreditText->SetText(TargetText);
}

void USLCreditTextWidget::SetTextFont(const FSlateFontInfo& TargetFont)
{
	CreditText->SetFont(TargetFont);
}
