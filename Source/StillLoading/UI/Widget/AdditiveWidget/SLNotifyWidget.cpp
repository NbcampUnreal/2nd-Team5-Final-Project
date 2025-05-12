// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLNotifyWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USLNotifyWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::EAW_NotifyWidget;
	WidgetInputMode = ESLInputModeType::EIM_GameAndUI;
	bIsVisibleCursor = false;

	OpenAnim = VisibleNotifyAnim;
	CloseAnim = InvisibleNotifyAnim;

	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLNotifyWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	PlayAnimation(OpenAnim);
	PlayUISound(ESLUISoundType::EUS_Notify);
}

void USLNotifyWidget::UpdateNotifyText(const FText& NewText)
{
	NotifyText->SetText(NewText);
}

void USLNotifyWidget::OnEndedOpenAnim()
{
	Super::OnEndedOpenAnim();

	PlayAnimation(CloseAnim);
}

void USLNotifyWidget::OnEndedCloseAnim()
{
	Super::OnEndedCloseAnim();

	CloseWidget();
}

void USLNotifyWidget::ApplyImageData()
{
	Super::ApplyImageData();
	//BackgroundImg->SetBrushFromTexture();
}

void USLNotifyWidget::ApplyFontData()
{
	Super::ApplyFontData();
	//NotifyText->SetFont();
}
