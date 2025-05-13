// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLIntroWidget.h"
#include "Kismet/GameplayStatics.h"

void USLIntroWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = false;

	OpenAnim = IntroOpenAnim;
	CloseAnim = IntroCloseAnim;

	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLIntroWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	PlayAnimation(OpenAnim);
	// Play Intro Sound?
}

void USLIntroWidget::OnEndedOpenAnim()
{
	Super::OnEndedOpenAnim();

	PlayAnimation(CloseAnim);
}

void USLIntroWidget::OnEndedCloseAnim()
{
	UGameplayStatics::OpenLevel(GetWorld(), "TestTitleLevel");
}
