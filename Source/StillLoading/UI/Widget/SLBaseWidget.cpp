// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SLBaseWidget.h"
#include "UI/SLUISubsystem.h"
#include "Animation/WidgetAnimation.h"


void USLBaseWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	UISubsystem = NewUISubsystem;

	if (IsValid(OpenAnim))
	{
		EndOpenAnimDelegate.BindDynamic(this, &ThisClass::OnEndedOpenAnim);
		BindToAnimationFinished(OpenAnim, EndOpenAnimDelegate);
	}

	if (IsValid(CloseAnim))
	{
		EndCloseAnimDelegate.BindDynamic(this, &ThisClass::OnEndedCloseAnim);
		BindToAnimationFinished(CloseAnim, EndCloseAnimDelegate);
	}
}

void USLBaseWidget::OnEndedOpenAnim()
{
	// TODO : Post Open Animation Function 
}

void USLBaseWidget::OnEndedCloseAnim()
{
	if (!CheckValidOfUISubsystem())
	{
		return;
	}

	// TODO : Pose Close Animation Function. Call Remove Widget and Post Processing From UISubsystem.
}

void USLBaseWidget::PlayOpenAnim()
{
	if (!IsValid(OpenAnim))
	{
		return;
	}

	PlayAnimation(OpenAnim);
}

void USLBaseWidget::PlayCloseAnim()
{
	if (!IsValid(CloseAnim))
	{
		return;
	}

	PlayAnimation(CloseAnim);
}

void USLBaseWidget::PlayUISound(ESLUISoundType SoundType)
{
	if (!CheckValidOfUISubsystem())
	{
		return;
	}

	// TODO : Call SFX Sound Play Function From UISubsystem.
}

bool USLBaseWidget::CheckValidOfUISubsystem()
{
	if (IsValid(UISubsystem))
	{
		return true;
	}

	if (!IsValid(GetGameInstance()))
	{
		return false;
	}

	UISubsystem = GetGameInstance()->GetSubsystem<USLUISubsystem>();

	if (!IsValid(UISubsystem))
	{
		return false;
	}

	return true;
}
