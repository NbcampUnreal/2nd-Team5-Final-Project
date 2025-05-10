// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLFadeWidget.h"
#include "Components/Image.h"

const FName USLFadeWidget::FadeImgName = "FadeImage";

void USLFadeWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetType = ESLAdditiveWidgetType::EAW_FadeWidget;

	OpenAnim = FadeInAnim;
	CloseAnim = FadeOutAnim;

	Super::InitWidget(NewUISubsystem, ChapterType);
}

void USLFadeWidget::ActivateWidget(ESLChapterType ChapterType)
{
	Super::ActivateWidget(ChapterType);

	if (bIsFadeIn)
	{
		PlayAnimation(OpenAnim);
	}
	else
	{
		PlayAnimation(CloseAnim);
	}
}

void USLFadeWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	bIsFadeIn = !bIsFadeIn;
}

void USLFadeWidget::OnEndedOpenAnim()
{
	Super::OnEndedOpenAnim();

	OnEndedCloseAnim();
}

void USLFadeWidget::OnEndedCloseAnim()
{
	Super::OnEndedCloseAnim();

	CloseWidget();
}

void USLFadeWidget::ApplyImageData()
{
	Super::ApplyImageData();

	checkf(ImageMap.Contains(FadeImgName), TEXT("Image Name is Not Contains"));
	checkf(IsValid(ImageMap[FadeImgName]), TEXT("Image Source is invalid"));
	FadeImage->SetBrushFromTexture(ImageMap[FadeImgName]);
}
