// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/LevelWidget/SLLevelWidget.h"
#include "SLInGameWidget.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;
class UCanvasPanel;

UCLASS()
class STILLLOADING_API USLInGameWidget : public USLLevelWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitWidget(USLUISubsystem* NewUISubsystem) override;
	virtual void DeactivateWidget() override;

	void SetIsTimerActivate(bool bIsActived);
	void SetIsPlayerStateActivate(bool bIsActived);
	void SetIsGameStateActivate(bool bIsActived);
	void SetIsHitEffectActivate(bool bIsActived);
	void SetIsBossStateActivate(bool bIsActived);

	void SetTimerText(int32 TimeSeconds);
	void SetHpValue(int32 MaxHp, int32 CurrentHp);
	void SetBossHpValue(int32 MaxHp, int32 CurrentHp);
	void SetGameStateText(const FText& StateText);

protected:
	virtual void ApplyImageData() override;
	virtual void ApplyFontData() override;

	void SetIsSubWidgetActivate(bool bIsActived, UWidgetAnimation* ActiveAnim, UWidgetAnimation* DeactiveAnim);
	void PlaySubWidgetAnim(UWidgetAnimation* PlayTargetAnim, UWidgetAnimation* StopTargetAnim = nullptr);

protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> HitEffectImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> TimerBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> GameStateBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> PlayerStateBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> GameStateText = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UProgressBar> HpBar = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UProgressBar> BossHpBar = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> HitEffectPanel = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ActiveTimerAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> DeactiveTimerAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ActiveGameStateAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> DeactiveGameStateAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ActivePlayerStateAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> DeactivePlayerStateAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ActiveBossStateAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> DeactiveBossStateAnim = nullptr;

	UPROPERTY(Meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ActiveHitEffectAnim = nullptr;

	static const FName HitEffectIndex;
	static const FName TimerBackIndex;
	static const FName GameStateBackIndex;
	static const FName PlayerStateBackIndex;
};
