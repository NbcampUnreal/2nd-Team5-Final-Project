// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "UI/SLUISubsystem.h"
#include "UI/Struct/SLLevelWidgetDataRow.h"

void USLLevelWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetOrder = 0;
	Super::InitWidget(NewUISubsystem);
}

void USLLevelWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	CheckValidOfUISubsystem();
	UISubsystem->SetLevelInputMode(WidgetInputMode, bIsVisibleCursor);

	Super::ActivateWidget(WidgetActivateBuffer);
}

void USLLevelWidget::RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType TargetWidgetType)
{
	CheckValidOfUISubsystem();
	UISubsystem->AddAdditveWidget(TargetWidgetType);
}