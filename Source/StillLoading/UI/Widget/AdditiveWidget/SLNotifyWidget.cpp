// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLNotifyWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USLNotifyWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::NotifyWidget;

	OpenAnim = VisibleNotifyAnim;
	CloseAnim = InvisibleNotifyAnim;

	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLNotifyWidget::DeactivateWidget()
{
	
}

void USLNotifyWidget::UpdateNotifyText(const FText& NewText)
{
	NotifyText->SetText(NewText);
}

void USLNotifyWidget::OnEndedOpenAnim()
{
	PlayAnimation(CloseAnim);
}

void USLNotifyWidget::OnEndedCloseAnim()
{
	CloseWidget();
}

void USLNotifyWidget::ApplyImageData()
{
	//BackgroundImg->SetBrushFromTexture();
}

void USLNotifyWidget::ApplyFontData()
{
	//NotifyText->SetFont();
}
