// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLCreditWidget.h"
#include "Animation/WidgetAnimation.h"
#include "UI/Widget/AdditiveWidget/SubWidget/SLCreditTextWidget.h"
#include "SaveLoad/SLSaveGameSubsystem.h"
#include "Components/Button.h"
#include "UI/Widget/SLButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"

void USLCreditWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_CreditWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 19;
	bIsVisibleCursor = true;
	
	Super::InitWidget(NewUISubsystem);

	CloseButton->InitButton();
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedCloseButton);
	CloseButton->SetButtonText(FText::FromString(FString::Printf(TEXT("확인"))));
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

void USLCreditWidget::ApplyFontData()
{
	Super::ApplyFontData();

}

void USLCreditWidget::ApplyTextData()
{
	Super::ApplyTextData();

}

bool USLCreditWidget::ApplyBackgroundImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyBackgroundImage(SlateBrush))
	{
		return false;
	}

	BackgroundImg->SetBrush(SlateBrush);

	return true;
}

void USLCreditWidget::OnClickedCloseButton()
{
	PlayUISound(ESLUISoundType::EUS_Click);

	USLSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USLSaveGameSubsystem>();
	SaveGameSubsystem->ResetGameData();
	MoveToLevelByType(ESLLevelNameType::ELN_Intro);
}