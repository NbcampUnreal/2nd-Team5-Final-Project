// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLCreditWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void USLCreditWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::EAW_CreditWidget;
	
	Super::InitWidget(NewUISubsystem, ChapterType);

	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);
}

void USLCreditWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	PlayAnimation(OpenAnim);
}

void USLCreditWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	PlayAnimation(CloseAnim);
}

void USLCreditWidget::ApplyImageData()
{

}

void USLCreditWidget::ApplyFontData()
{

}