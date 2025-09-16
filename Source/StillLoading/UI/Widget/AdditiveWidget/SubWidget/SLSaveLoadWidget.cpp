// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SubWidget/SLSaveLoadWidget.h"

void USLSaveLoadWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	Super::InitWidget(NewUISubsystem);
}

void USLSaveLoadWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	OnUpdatedSettingValue();
}

void USLSaveLoadWidget::OnUpdatedSettingValue()
{
	Super::OnUpdatedSettingValue();

}

void USLSaveLoadWidget::ApplyTextData()
{
	Super::ApplyTextData();
}
