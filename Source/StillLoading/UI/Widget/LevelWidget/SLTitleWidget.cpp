// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLTitleWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SLUISubsystem.h"

const FName USLTitleWidget::BackgroundImgIndex = "BackgroundImg";
const FName USLTitleWidget::TitleTextImgIndex = "TitleTextImg";
const FName USLTitleWidget::ButtonsBackgroundImgIndex = "ButtonsBackground";
const FName USLTitleWidget::ButtonImgIndex = "ButtonImg";

void USLTitleWidget::InitWidget(USLUISubsystem* NewUISubsystem, ESLChapterType ChapterType)
{
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	bIsVisibleCursor = true;
	
	Super::InitWidget(NewUISubsystem, ChapterType);

	StartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedStartButton);
	OptionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedOptionButton);
	QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuitButton);
}

void USLTitleWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLTitleWidget::ApplyImageData()
{
	Super::ApplyImageData();

	if (ImageMap.Contains(BackgroundImgIndex) && IsValid(ImageMap[BackgroundImgIndex]))
	{
		BackgroundImg->SetBrushFromTexture(ImageMap[BackgroundImgIndex]);
	}
	
	if (ImageMap.Contains(TitleTextImgIndex) && IsValid(ImageMap[TitleTextImgIndex]))
	{
		TitleTextImg->SetBrushFromTexture(ImageMap[TitleTextImgIndex]);
	}
	
	if (ImageMap.Contains(ButtonsBackgroundImgIndex) && IsValid(ImageMap[ButtonsBackgroundImgIndex]))
	{
		ButtonsBackground->SetBrushFromTexture(ImageMap[ButtonsBackgroundImgIndex]);
	}
	
	if (ImageMap.Contains(ButtonImgIndex) && IsValid(ImageMap[ButtonImgIndex]))
	{
		StartButton->WidgetStyle.Normal.SetResourceObject(ImageMap[ButtonImgIndex]);
		StartButton->WidgetStyle.Hovered.SetResourceObject(ImageMap[ButtonImgIndex]);
		StartButton->WidgetStyle.Pressed.SetResourceObject(ImageMap[ButtonImgIndex]);

		OptionButton->WidgetStyle.Normal.SetResourceObject(ImageMap[ButtonImgIndex]);
		OptionButton->WidgetStyle.Hovered.SetResourceObject(ImageMap[ButtonImgIndex]);
		OptionButton->WidgetStyle.Pressed.SetResourceObject(ImageMap[ButtonImgIndex]);

		QuitButton->WidgetStyle.Normal.SetResourceObject(ImageMap[ButtonImgIndex]);
		QuitButton->WidgetStyle.Hovered.SetResourceObject(ImageMap[ButtonImgIndex]);
		QuitButton->WidgetStyle.Pressed.SetResourceObject(ImageMap[ButtonImgIndex]);
	}
}

void USLTitleWidget::ApplyFontData()
{
	Super::ApplyFontData();

	/*TitleText->SetFont(FontInfo);
	StartText->SetFont(FontInfo);
	OptionText->SetFont(FontInfo);
	QuitText->SetFont(FontInfo);*/
}

void USLTitleWidget::OnClickedStartButton()
{
	// TODO : Request Move To MapList Level
	// Test Code : Add Map List Widget To Viewport
	//CheckValidOfUISubsystem();
	//UISubsystem->ActivateFade(false);

	PlayUISound(ESLUISoundType::EUS_Click);
	UGameplayStatics::OpenLevel(GetWorld(), "TestMapListLevel");
}

void USLTitleWidget::OnClickedOptionButton()
{
	RequestAddedWidgetToUISubsystem(ESLAdditiveWidgetType::EAW_OptionWidget);
	PlayUISound(ESLUISoundType::EUS_Click);
}

void USLTitleWidget::OnClickedQuitButton()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
	PlayUISound(ESLUISoundType::EUS_Click);
}
