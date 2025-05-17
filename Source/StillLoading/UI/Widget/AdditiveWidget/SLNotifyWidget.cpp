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
	PlayAnimation(OpenAnim);
	PlayUISound(ESLUISoundType::EUS_Notify);
}

void USLNotifyWidget::UpdateNotifyText(ESLGameMapType MapType, ESLNotifyType NotiType)
{
	CheckValidOfTextPoolSubsystem();
	const UDataTable* NotifyDataTable = TextPoolSubsystem->GetNotifyTextPool();
	
	TArray<FSLNotifyTextPoolDataRow*> NotifyDataArray;
	NotifyDataTable->GetAllRows(TEXT("Notify Text Context"), NotifyDataArray);

	for (const FSLNotifyTextPoolDataRow* NotifyData : NotifyDataArray)
	{
		if (NotifyData->Chapter == CurrentChapter &&
			NotifyData->TextMap.Contains(MapType))
		{
			NotifyText->SetText(NotifyData->TextMap[MapType].NotifyMap[NotiType]);
			break;
		}
	}
}

void USLNotifyWidget::OnEndedOpenAnim()
{
	Super::OnEndedOpenAnim();

	PlayAnimation(CloseAnim);
}

void USLNotifyWidget::OnEndedCloseAnim()
{
	Super::OnEndedCloseAnim();

	CloseWidget();
}

void USLNotifyWidget::ApplyFontData()
{
	Super::ApplyFontData();

	//NotifyText->SetFont(FontInfo);
}

bool USLNotifyWidget::ApplyBackgroundImage()
{
	if (!Super::ApplyBackgroundImage())
	{
		return false;
	}

	BackgroundImg->SetBrushFromTexture(PublicImageMap[ESLPublicWidgetImageType::EPWI_Background]);
	return true;
}
