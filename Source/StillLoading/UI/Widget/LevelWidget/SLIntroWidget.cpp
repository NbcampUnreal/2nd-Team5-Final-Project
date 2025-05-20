// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLIntroWidget.h"

void USLIntroWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = false;

	OpenAnim = IntroOpenAnim;
	CloseAnim = IntroCloseAnim;

	Super::InitWidget(NewUISubsystem);
}

void USLIntroWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

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
	MoveToLevelByType(ESLLevelNameType::ELN_Title, false);
}
