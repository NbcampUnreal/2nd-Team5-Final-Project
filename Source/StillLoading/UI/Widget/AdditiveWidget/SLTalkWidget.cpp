// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLTalkWidget.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

const FName USLTalkWidget::TalkBackImgIndex = "TalkBackImg";
const FName USLTalkWidget::NameBackImgIndex = "NameBackImg";

void USLTalkWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_TalkWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 10;
	bIsVisibleCursor = true;

	ParentNamePanel = NamePanel;
	ParentNameText = NameText;
	ParentNextButton = NextButton;
	ParentTalkText = TalkText;

	Super::InitWidget(NewUISubsystem);
}

void USLTalkWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	UpdateTalkState(WidgetActivateBuffer.TargetTalk, WidgetActivateBuffer.TargetIndex);
	Super::ActivateWidget(WidgetActivateBuffer);	
}

void USLTalkWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	TalkArray.Empty();
	NameArray.Empty();
	OnEndedCloseAnim();
}

void USLTalkWidget::UpdateTalkState(ESLTalkTargetType TalkTargetType, int32 TargetIndex)
{
	CheckValidOfTextPoolSubsystem();

	const UDataTable* TalkDataTable = TextPoolSubsystem->GetTalkTextPool();

	TArray<FSLTalkTextPoolDataRow*> TalkDataArray; 
	TalkDataTable->GetAllRows(TEXT("Talk Text Context"), TalkDataArray);

	for (const FSLTalkTextPoolDataRow* TalkData : TalkDataArray)
	{
		if (TalkData->TalkTarget == TalkTargetType &&
			TalkData->TalkMap.Contains(TargetIndex))
		{
			TalkArray = TalkData->TalkMap[TargetIndex].TalkTextArray;
			NameArray = TalkData->TalkMap[TargetIndex].TalkOwnArray;
			CurrentTalkType = TalkTargetType;
			CurrentTalkIndex = TargetIndex;
			break;
		}
	}
}

void USLTalkWidget::ApplyImageData()
{
	Super::ApplyImageData();

	if (ImageMap.Contains(TalkBackImgIndex) &&
		IsValid(ImageMap[TalkBackImgIndex]))
		TalkBack->SetBrushFromTexture(ImageMap[TalkBackImgIndex]);

	if (ImageMap.Contains(NameBackImgIndex) &&
		IsValid(ImageMap[NameBackImgIndex]))
		NameBack->SetBrushFromTexture(ImageMap[NameBackImgIndex]);
}

void USLTalkWidget::ApplyFontData()
{
	Super::ApplyFontData();

	/*NameText->SetFont(FontInfo);
	TalkText->SetFont(FontInfo);*/
}

void USLTalkWidget::ApplyTextData()
{
	Super::ApplyTextData();

	if (!this->IsInViewport())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);
	UpdateTalkState(CurrentTalkType, CurrentTalkIndex);
	ChangeTargetText();
	PrintTalkText();
}