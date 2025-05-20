// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLFadeWidget.h"
#include "Components/Image.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "SubSystem/SLLevelTransferSubsystem.h"

void USLFadeWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_FadeWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 20;
	bIsVisibleCursor = false;

	OpenAnim = FadeInAnim;
	CloseAnim = FadeOutAnim;

	Super::InitWidget(NewUISubsystem);
}

void USLFadeWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);

	bIsMoveLevel = WidgetActivateBuffer.bIsMoveLevel;

	if (WidgetActivateBuffer.bIsFade)
	{
		if (IsPlayingAnimation())
		{
			StopAllAnimations();
		}

		PlayAnimation(OpenAnim);
	}
	else
	{
		if (IsPlayingAnimation())
		{
			StopAllAnimations();
		}

		PlayAnimation(CloseAnim);
	}
}

void USLFadeWidget::OnEndedOpenAnim()
{
	Super::OnEndedOpenAnim();

	CloseWidget();
	this->RemoveFromViewport();
}

void USLFadeWidget::OnEndedCloseAnim()
{
	if (bIsMoveLevel)
	{
		USLLevelTransferSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<USLLevelTransferSubsystem>();
		checkf(IsValid(LevelSubsystem), TEXT("Level Subsystem is invalid"));
		LevelSubsystem->PostFadeOut();
	}
}

bool USLFadeWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_Fade) ||
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_Fade]))
	{
		return false;
	}
	
	FadeImage->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_Fade]);

	return true;
}