// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AdditiveWidget/SLNotifyWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "SubSystem/SLTextPoolSubsystem.h"
#include "SubSystem/Struct/SLTextPoolDataRows.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"

void USLNotifyWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetType = ESLAdditiveWidgetType::EAW_NotifyWidget;
	WidgetInputMode = ESLInputModeType::EIM_GameAndUI;
	bIsVisibleCursor = false;

	OpenAnim = VisibleNotifyAnim;
	CloseAnim = InvisibleNotifyAnim;

	Super::InitWidget(NewUISubsystem);
}

void USLNotifyWidget::ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::ActivateWidget(WidgetActivateBuffer);
	
	UpdateNotifyText(WidgetActivateBuffer.TargetMap, WidgetActivateBuffer.TargetNotify);

	if (IsAnimationPlaying(CloseAnim))
	{
		StopAnimation(CloseAnim);
	}

	PlayAnimation(OpenAnim);
	PlayUISound(ESLUISoundType::EUS_Notify);
}

void USLNotifyWidget::UpdateNotifyText(ESLGameMapType MapType, const FName& NotiName)
{
	CheckValidOfTextPoolSubsystem();
	const UDataTable* NotifyDataTable = TextPoolSubsystem->GetNotifyTextPool();
	
	TArray<FSLNotifyTextPoolDataRow*> NotifyDataArray;
	NotifyDataTable->GetAllRows(TEXT("Notify Text Context"), NotifyDataArray);

	for (const FSLNotifyTextPoolDataRow* NotifyData : NotifyDataArray)
	{
		if (NotifyData->TextMap.Contains(MapType))
		{
			if (NotifyData->TextMap[MapType].NotifyMap.Contains(NotiName))
			{
				NotifyText->SetText(NotifyData->TextMap[MapType].NotifyMap[NotiName]);
			}
			
			break;
		}
	}
}

void USLNotifyWidget::OnEndedOpenAnim()
{
	Super::OnEndedOpenAnim();

	if (IsAnimationPlaying(OpenAnim))
	{
		return;
	}

	PlayAnimation(CloseAnim);
}

void USLNotifyWidget::OnEndedCloseAnim()
{
	Super::OnEndedCloseAnim();

	if (!IsAnimationPlaying(OpenAnim))
	{
		CloseWidget();
	}
}

bool USLNotifyWidget::ApplyBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyBorderImage(SlateBrush))
	{
		return false;
	}

	BackgroundImg->SetBrush(SlateBrush);
	return true;
}