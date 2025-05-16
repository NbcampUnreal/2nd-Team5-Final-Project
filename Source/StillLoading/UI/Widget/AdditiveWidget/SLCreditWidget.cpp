// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLCreditWidget.h"
#include "Animation/WidgetAnimation.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLCreditTextWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"

void USLCreditWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_CreditWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 19;
	bIsVisibleCursor = true;
	
	Super::InitWidget(NewUISubsystem);

	CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseWidget);
}

void USLCreditWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	PlayAnimation(OpenAnim);

	// TODO : Get Credit Text Pool From TextSubsystem. And Create CreditTextWidget - Create Count = Text Pool Size
	/*for (int32 i = 0; i < 20; ++i)
	{
		USLCreditTextWidget* NewCreditText = CreateWidget<USLCreditTextWidget>(this, CreditTextWidgetClass);
		CreditBox->AddChild(NewCreditText);
	}

	CreditBox->ScrollToEnd();*/
}

void USLCreditWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	if (IsValid(CloseAnim))
	{
		PlayAnimation(CloseAnim);
	}
	else
	{
		OnEndedCloseAnim();
	}
}

void USLCreditWidget::ApplyImageData()
{

}

void USLCreditWidget::ApplyFontData()
{
	Super::ApplyFontData();

	//TitleText->SetFont(FontInfo);
}