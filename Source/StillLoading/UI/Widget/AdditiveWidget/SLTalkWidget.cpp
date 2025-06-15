// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLTalkWidget.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "UI/Widget/SLButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"

void USLTalkWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_TalkWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 10;
	bIsVisibleCursor = true;

	ParentNextButton = NextButton;
	ParentSkipButton = SkipButton;
	ParentFastButton = FastButton;
	ParentAcceptButton = AcceptButton;
	ParentRejectButton = RejectButton;

	Super::InitWidget(NewUISubsystem);
}

void USLTalkWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	ChangeTalkLayer(WidgetActivateBuffer.CurrentChapter);
	UpdateTalkState(WidgetActivateBuffer.TargetTalk, WidgetActivateBuffer.TargetName, WidgetActivateBuffer.TalkName);

	Super::ActivateWidget(WidgetActivateBuffer);	
}

void USLTalkWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	TalkArray.Empty();
	NameArray.Empty();
	OnEndedCloseAnim();
}

void USLTalkWidget::UpdateTalkState(ESLTalkTargetType TalkTargetType, const FName& TargetName, const FName& TalkName)
{
	CheckValidOfTextPoolSubsystem();

	const UDataTable* TalkDataTable = TextPoolSubsystem->GetTalkTextPool();

	TArray<FSLTalkTextPoolDataRow*> TalkDataArray; 
	TalkDataTable->GetAllRows(TEXT("Talk Text Context"), TalkDataArray);

	for (const FSLTalkTextPoolDataRow* TalkData : TalkDataArray)
	{
		if (TalkData->TalkTarget == TalkTargetType &&
			TalkData->TargetName == TargetName &&
			TalkData->TalkMap.Contains(TalkName))
		{
			TalkArray = TalkData->TalkMap[TalkName].TalkTextArray;
			NameArray = TalkData->TalkMap[TalkName].TalkOwnArray;
			CurrentTalkType = TalkTargetType;
			CurrentTalkTarget = TargetName;
			CurrentTalkName = TalkName;
			break;
		}
	}
}

void USLTalkWidget::ChangeTalkLayer(ESLChapterType ChapterType)
{
	if (ChapterType == ESLChapterType::EC_Chapter3)
	{
		ParentNamePanel = MidNamePanel;
		ParentNameText = MidNameText;
		ParentTalkText = MidTalkText;

		WidgetSwitcher->SetActiveWidgetIndex(1);
	}
	else
	{
		ParentNamePanel = LeftNamePanel;
		ParentNameText = LeftNameText;
		ParentTalkText = LeftTalkText;

		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}

void USLTalkWidget::ApplyTextData()
{
	Super::ApplyTextData();

	if (!this->IsInViewport())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TextPrintTimer);
	UpdateTalkState(CurrentTalkType, CurrentTalkTarget, CurrentTalkName);
	ChangeTargetText();
	PrintTalkText();
}

bool USLTalkWidget::ApplyTextBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyTextBorderImage(SlateBrush))
	{
		return false;
	}
	
	LeftTalkBack->SetBrush(SlateBrush);
	MidTalkBack->SetBrush(SlateBrush);
	
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_NameBorder) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_NameBorder]))
	{
		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_NameBorder]);
		LeftNameBack->SetBrush(SlateBrush);
		MidNameBack->SetBrush(SlateBrush);
	}
	
	return true;
}