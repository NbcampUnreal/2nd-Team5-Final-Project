// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLTalkWidget.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

void USLTalkWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_TalkWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 10;
	bIsVisibleCursor = true;

	ParentNamePanel = NamePanel;
	ParentNameText = NameText;
	ParentNextButton = NextButton;
	ParentSkipButton = SkipButton;
	ParentFastButton = FastButton;
	ParentTalkText = TalkText;

	Super::InitWidget(NewUISubsystem);
}

void USLTalkWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
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
	
	TalkBack->SetBrush(SlateBrush);
	
	if (PublicAssetMap.Contains(ESLPublicWidgetImageType::EPWI_NameBorder) &&
		IsValid(PublicAssetMap[ESLPublicWidgetImageType::EPWI_NameBorder]))
	{
		SlateBrush.SetResourceObject(PublicAssetMap[ESLPublicWidgetImageType::EPWI_NameBorder]);
		NameBack->SetBrush(SlateBrush);
	}
	
	return true;
}

bool USLTalkWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
{
	if (!Super::ApplyButtonImage(ButtonStyle))
	{
		return false;
	}

	SkipButton->SetStyle(ButtonStyle);
	FastButton->SetStyle(ButtonStyle);

	return true;
}
