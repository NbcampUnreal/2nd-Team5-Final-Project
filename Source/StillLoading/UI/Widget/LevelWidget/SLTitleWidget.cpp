// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLTitleWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void USLTitleWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;

	Super::InitWidget(NewUISubsystem, ChapterType);

	StartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedStartButton);
	OptionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedOptionButton);
	QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuitButton);
}

void USLTitleWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLTitleWidget::ApplyImageData()
{
	Super::ApplyImageData();

}

void USLTitleWidget::ApplyFontData()
{
	Super::ApplyFontData();

}

void USLTitleWidget::OnClickedStartButton()
{
	// TODO : Request Move To MapList Level
	// Test Code : Add Map List Widget To Viewport
}

void USLTitleWidget::OnClickedOptionButton()
{
	RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType::EAW_OptionWidget);
}

void USLTitleWidget::OnClickedQuitButton()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}
