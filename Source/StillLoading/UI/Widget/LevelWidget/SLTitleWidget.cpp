// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLTitleWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SLUISubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "SubSystem/SLTextPoolSubsystem.h"

const FName USLTitleWidget::TitleTextIndex = "TitleText";
const FName USLTitleWidget::StartButtonIndex = "StartButton";
const FName USLTitleWidget::OptionButtonIndex = "OptionButton";
const FName USLTitleWidget::QuitButtonIndex = "QuitButton";

void USLTitleWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_GameAndUI;
	bIsVisibleCursor = true;
	
	Super::InitWidget(NewUISubsystem);

	StartButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedStartButton);
	OptionButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedOptionButton);
	QuitButton->OnClicked.AddDynamic(this, &ThisClass::OnClickedQuitButton);
}

void USLTitleWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLTitleWidget::ApplyTextData()
{
	Super::ApplyTextData();

	CheckValidOfTextPoolSubsystem();
	const UDataTable* TextPool = TextPoolSubsystem->GetUITextPool();

	TArray<FSLUITextPoolDataRow*> TempArray;
	TextPool->GetAllRows(TEXT("UI Textpool Data ConText"), TempArray);

	TMap<FName, FSLUITextData> OptionTextMap;

	for (const FSLUITextPoolDataRow* UITextPool : TempArray)
	{
		if (UITextPool->TargetWidget == ESLTargetWidgetType::ETW_Title)
		{
			OptionTextMap = UITextPool->TextMap;
			break;
		}
	}

	TitleText->SetText(OptionTextMap[TitleTextIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	StartText->SetText(OptionTextMap[StartButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	OptionText->SetText(OptionTextMap[OptionButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
	QuitText->SetText(OptionTextMap[QuitButtonIndex].ChapterTextMap[ESLChapterType::EC_Intro]);
}

bool USLTitleWidget::ApplyBackgroundImage()
{
	if (!Super::ApplyBackgroundImage())
	{
		return false;
	}
	
	BackgroundImg->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_Background]);

	return true;
}

bool USLTitleWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyButtonImage(ButtonStyle))
	{
		return false;
	}

	StartButton->SetStyle(ButtonStyle);
	OptionButton->SetStyle(ButtonStyle);
	QuitButton->SetStyle(ButtonStyle);

	return true;
}

bool USLTitleWidget::ApplyBorderImage()
{
	if (!PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_NormalBorder) ||
		(PublicImageMap.Contains(ESLPublicWidgetImageType::EPWI_NormalBorder) &&
		!IsValid(PublicImageMap[ESLPublicWidgetImageType::EPWI_NormalBorder])))
	{
		BackgroundBorder->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!Super::ApplyBorderImage())
	{
		return false;
	}

	//TitleTextImg->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_NormalBorder]);
	//ButtonsBackground->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_NormalBorder]);

	if (BackgroundBorder->GetVisibility() == ESlateVisibility::Visible)
	{
		BackgroundBorder->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_NormalBorder]);
		BackgroundImg->SetRenderScale(FVector2D(0.95f, 0.95f));

		FLinearColor BackColor;

		BackColor.R = 0.0f;
		BackColor.G = 0.0f;
		BackColor.B = 0.0f;

		if (CurrentChapter == ESLChapterType::EC_Intro ||
			CurrentChapter == ESLChapterType::EC_Chapter2D)
		{
			BackColor.A = 1.0f;
		}
		else if (CurrentChapter == ESLChapterType::EC_Chapter2_5D)
		{
			BackColor.A = 0.5f;
		}

		BackgroundImg->SetColorAndOpacity(BackColor);
	}
	
	return true;
}

void USLTitleWidget::OnClickedStartButton()
{
	PlayUISound(ESLUISoundType::EUS_Click);
	MoveToLevelByType(ESLLevelNameType::ELN_MapList);
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
