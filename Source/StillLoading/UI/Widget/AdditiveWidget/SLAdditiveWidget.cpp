// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLAdditiveWidget.h"
#include "UI/SLUISubsystem.h"
#include "UI/Struct/SLWidgetImageDataRow.h"

ESLAdditiveWidgetType USLAdditiveWidget::GetWidgetType() const
{
	return WidgetType;
}

void USLAdditiveWidget::CloseWidget()
{
	CheckValidOfUISubsystem();
	UISubsystem->RemoveCurrentAdditiveWidget(WidgetType);
}
