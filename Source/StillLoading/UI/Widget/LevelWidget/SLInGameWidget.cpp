// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LevelWidget/SLInGameWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"

const FName USLInGameWidget::HitEffectIndex = "HitEffectImg";
const FName USLInGameWidget::TimerBackIndex = "TimerBackImg";
const FName USLInGameWidget::GameStateBackIndex = "GameStateBackImg";
const FName USLInGameWidget::PlayerStateBackIndex = "PlayerStateBackImg";

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

void USLInGameWidget::ApplyImageData()
{
	Super::ApplyImageData();

	if (ImageMap.Contains(HitEffectIndex) &&
		IsValid(ImageMap[HitEffectIndex])) 
		HitEffectImg->SetBrushFromTexture(ImageMap[HitEffectIndex]);

	if (ImageMap.Contains(TimerBackIndex) &&
		IsValid(ImageMap[TimerBackIndex]))
		TimerBack->SetBrushFromTexture(ImageMap[TimerBackIndex]);

	if (ImageMap.Contains(GameStateBackIndex) &&
		IsValid(ImageMap[GameStateBackIndex]))
		GameStateBack->SetBrushFromTexture(ImageMap[GameStateBackIndex]);

	if (ImageMap.Contains(PlayerStateBackIndex) && 
		IsValid(ImageMap[PlayerStateBackIndex]))
		PlayerStateBack->SetBrushFromTexture(ImageMap[PlayerStateBackIndex]);
}

void USLInGameWidget::ApplyFontData()
{
	Super::ApplyFontData();

	//TimerText->SetFont(FontInfo);
	//GameStateText->SetFont(FontInfo);
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
