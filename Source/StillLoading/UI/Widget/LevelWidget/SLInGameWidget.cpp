// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLInGameWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"
#include "UI/Widget/SLWidgetPrivateDataAsset.h"
#include "UI/Struct/SLWidgetActivateBuffer.h"

void USLInGameWidget::InitWidget(USLUISubsystem* NewUISubsystem)
{
	WidgetInputMode = ESLInputModeType::EIM_GameOnly;
	bIsVisibleCursor = false;

	Super::InitWidget(NewUISubsystem);

}

void USLInGameWidget::DeactivateWidget()
{
	Super::DeactivateWidget();

	OnEndedCloseAnim();
}

void USLInGameWidget::SetIsTimerActivate(bool bIsActived)
{
	SetIsSubWidgetActivate(bIsActived, ActiveTimerAnim, DeactiveTimerAnim);
}

void USLInGameWidget::SetIsPlayerStateActivate(bool bIsActived)
{
	SetIsSubWidgetActivate(bIsActived, ActivePlayerStateAnim, DeactivePlayerStateAnim);
}

void USLInGameWidget::SetIsGameStateActivate(bool bIsActived)
{
	SetIsSubWidgetActivate(bIsActived, ActiveGameStateAnim, DeactiveGameStateAnim);
}

void USLInGameWidget::SetIsBossStateActivate(bool bIsActived)
{
	SetIsSubWidgetActivate(bIsActived, ActiveBossStateAnim, DeactiveBossStateAnim);
}

void USLInGameWidget::SetIsHitEffectActivate(bool bIsActived)
{
	if (bIsActived)
	{
		PlaySubWidgetAnim(ActiveHitEffectAnim);
	}
	else
	{
		HitEffectPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USLInGameWidget::SetTimerText(int32 TimeSeconds)
{
	int32 TextMin = TimeSeconds / 60;
	int32 TextSec = TimeSeconds % 60;

	TimerText->SetText(FText::FromString(FString::Printf(TEXT("%2d : %2d"), TextMin, TextSec)));
}

void USLInGameWidget::SetHpValue(int32 MaxHp, int32 CurrentHp)
{
	float PerHp = (float)CurrentHp / (float)MaxHp;

	HpBar->SetPercent(PerHp);
}

void USLInGameWidget::SetBossHpValue(int32 MaxHp, int32 CurrentHp)
{
	float PerHp = (float)CurrentHp / (float)MaxHp;

	BossHpBar->SetPercent(PerHp);
}

void USLInGameWidget::SetGameStateText(const FText& StateText)
{
	GameStateText->SetText(StateText);
}

void USLInGameWidget::FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer)
{
	Super::FindWidgetData(WidgetActivateBuffer);

	if (IsValid(WidgetActivateBuffer.WidgetPrivateData))
	{
		USLInGamePrivateDataAsset* PrivateData = Cast<USLInGamePrivateDataAsset>(WidgetActivateBuffer.WidgetPrivateData);
		PrivateImageMap.Empty();
		PrivateImageMap = PrivateData->GetInGameImageByChapter(WidgetActivateBuffer.CurrentChapter).InGameImageMap;
	}
}

void USLInGameWidget::ApplyFontData()
{
	Super::ApplyFontData();
}

bool USLInGameWidget::ApplyBorderImage(FSlateBrush& SlateBrush)
{
	if (!Super::ApplyBorderImage(SlateBrush))
	{
		return false;
	}

	TimerBack->SetBrush(SlateBrush);
	PlayerStateBack->SetBrush(SlateBrush);
	GameStateBack->SetBrush(SlateBrush);

	return true;
}

bool USLInGameWidget::ApplyOtherImage()
{
	Super::ApplyOtherImage();

	if (PrivateImageMap.Contains(ESLInGamePrivateImageType::EGPI_HitEffect) &&
		IsValid(PrivateImageMap[ESLInGamePrivateImageType::EGPI_HitEffect]))
	{
		HitEffectImg->SetBrushFromTexture(PrivateImageMap[ESLInGamePrivateImageType::EGPI_HitEffect]);
	}

	return true;
}

void USLInGameWidget::SetIsSubWidgetActivate(bool bIsActived, UWidgetAnimation* ActiveAnim, UWidgetAnimation* DeactiveAnim)
{
	if (bIsActived)
	{
		PlaySubWidgetAnim(ActiveAnim, DeactiveAnim);
	}
	else
	{
		PlaySubWidgetAnim(DeactiveAnim, ActiveAnim);
	}
}

void USLInGameWidget::PlaySubWidgetAnim(UWidgetAnimation* PlayTargetAnim, UWidgetAnimation* StopTargetAnim)
{
	if (IsAnimationPlaying(PlayTargetAnim))
	{
		return;
	}

	if (IsValid(StopTargetAnim) && 
		IsAnimationPlaying(StopTargetAnim))
	{
		StopAnimation(StopTargetAnim);
	}

	PlayAnimation(PlayTargetAnim);
}
