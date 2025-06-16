// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLStoryWidget.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"

void USLStoryWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_StoryWidget;
	WidgetInputMode = ESLInputModeType::EIM_UIOnly;
	WidgetOrder = 12;
	bIsVisibleCursor = false;

	//ParentNamePanel = NamePanel;
	//ParentNameText = NameText;
	//ParentNextButton = NextButton;
	//ParentSkipButton = SkipButton;
	//ParentFastButton = FastButton;
	//ParentTalkText = StoryText;

	if (IsValid(DissolveTextAnim))
	{
		EndDissolveAnimDelegate.BindDynamic(this, &ThisClass::ChangeDissolveText);
		BindToAnimationFinished(DissolveTextAnim, EndDissolveAnimDelegate);
	}

	if (IsValid(ChangeTextAnim))
	{
		EndChangeAnimDelegate.BindDynamic(this, &ThisClass::OnChangedText);
		BindToAnimationFinished(ChangeTextAnim, EndChangeAnimDelegate);
	}

	Super::InitWidget(NewUISubsystem);
}

void USLStoryWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	UpdateStoryState(WidgetActivateBuffer.TargetStory, WidgetActivateBuffer.TalkName);
	Super::ActivateWidget(WidgetActivateBuffer);

	CurrentTextIndex = 0;
	ChangeTargetText();
}

void USLStoryWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	StoryArray.Empty();
	NameArray.Empty();
	OnEndedCloseAnim();
}

void USLStoryWidget::SetNextStoryText()
{
	++CurrentTextIndex;

	if (CurrentTextIndex >= StoryArray.Num())
	{
		OnStoryEnded.Broadcast();
		CloseWidget();
		OnEndedCloseAnim();
		return;
	}

	ChangeTargetText();
}

void USLStoryWidget::SetStoryVisibility(bool bIsVisible)
{
	if (bIsVisible)
	{
		WidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		WidgetSwitcher->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USLStoryWidget::UpdateStoryState(ESLStoryType TargetStoryType, const FName& StoryName)
{
	CheckValidOfTextPoolSubsystem();

	const UDataTable* StoryDataTable = TextPoolSubsystem->GetStoryTextPool();

	TArray<FSLStoryTextPoolDataRow*> StoryDataArray;
	StoryDataTable->GetAllRows(TEXT("Story Text Context"), StoryDataArray);

	for (const FSLStoryTextPoolDataRow* TalkData : StoryDataArray)
	{
		if (TalkData->TextMap.Contains(TargetStoryType))
		{
			StoryArray = TalkData->TextMap[TargetStoryType].TalkTextArray;
			NameArray = TalkData->TextMap[TargetStoryType].TalkOwnArray;
			CurrentStoryType = TargetStoryType;
			CurrentStoryName = StoryName;
			break;
		}
	}
}

void USLStoryWidget::ChangeTargetText()
{
	FName TargetName = NameArray[CurrentTextIndex];
	FText ResultText = StoryArray[CurrentTextIndex];

	if (TargetName == "MidStory")
	{
		TargetText = MidStoryText;
		WidgetSwitcher->SetActiveWidgetIndex(1);

		if (bIsMidFirst)
		{
			TargetText->SetText(ResultText);
			PlayAnimation(ChangeTextAnim);
			bIsMidFirst = false;
		}
		else
		{
			CurrentText = ResultText;
			PlayAnimation(DissolveTextAnim);
		}
	}
	else
	{
		if (TargetName != "None")
		{
			ResultText = FText::FromString(FString::Printf(TEXT("%s : %s"), *TargetName.ToString(), *ResultText.ToString()));
		}

		TargetText = BottomStoryText;
		WidgetSwitcher->SetActiveWidgetIndex(0);

		TargetText->SetText(ResultText);
		bIsMidFirst = true;
	}
}

void USLStoryWidget::ChangeDissolveText()
{
	TargetText->SetText(CurrentText);
	
	if (CurrentTextIndex < NameArray.Num())
	{
		if (NameArray[CurrentTextIndex] == "MidStory")
		{
			PlayAnimation(ChangeTextAnim);
		}
	}
}

void USLStoryWidget::OnChangedText()
{

}

void USLStoryWidget::ApplyTextData()
{
	Super::ApplyTextData();

	if (!this->IsInViewport())
	{
		return;
	}

	UpdateStoryState(CurrentStoryType, CurrentStoryName);
	ChangeTargetText();
}

bool USLStoryWidget::ApplyTextBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyTextBorderImage(SlateBrush))
	{
		return false;
	}

	BottomStoryBack->SetBrush(SlateBrush);
	MidStoryBack->SetBrush(SlateBrush);

	return true;
}

//bool USLStoryWidget::ApplyButtonImage(FButtonStyle& ButtonStyle)
//{
//	if (!Super::ApplyButtonImage(ButtonStyle))
//	{
//		return false;
//	}
//
//	SkipButton->SetStyle(ButtonStyle);
//	FastButton->SetStyle(ButtonStyle);
//
//	return true;
//}
