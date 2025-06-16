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
	virtual void ActivateWidget(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;
	virtual void DeactivateWidget() override;

	void SetIsTimerActivate(bool bIsActived);
	void SetIsPlayerStateActivate(bool bIsActived, bool bIsVisibleSpecial);
	void SetIsObjectiveActivate();
	void SetIsHitEffectActivate(bool bIsActived);
	void SetIsBossStateActivate(bool bIsActived);

	UFUNCTION()
	void SetTimerText(int32 TimeSeconds);
	void SetHpValue(float MaxHp, float CurrentHp);
	void SetEffectValue(float MaxHp, float CurrentHp);
	void SetSpecialValue(float MaxValue, float CurrentValue);
	void SetBossHpValue(float MaxHp, float CurrentHp);
	void SetObjectiveText(const FName& ObjectiveName);
	void SetObjectiveByCounter(const FName& ObjectiveName, int32 MaxCount, int32 CurrentCount);

	const TMap<ESLInGameActivateType, bool>& GetActivateUIMap();

protected:
	virtual void FindWidgetData(const FSLWidgetActivateBuffer& WidgetActivateBuffer) override;

	virtual void ApplyFontData() override;
	virtual bool ApplyBorderImage(FSlateBrush& SlateBrush) override;
	virtual bool ApplyOtherImage() override;

	void SetIsSubWidgetActivate(bool bIsActived, UWidgetAnimation* ActiveAnim, UWidgetAnimation* DeactiveAnim);
	void PlaySubWidgetAnim(UWidgetAnimation* PlayTargetAnim, UWidgetAnimation* StopTargetAnim = nullptr);

private:
	void ApplyTimerImage();
	void ApplyProgressBarImage();

protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> HitEffectImg = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> TimerBack = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> TimerBorder = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> TimerHands = nullptr;

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
	TObjectPtr<UProgressBar> SpecialBar = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UProgressBar> BossHpBar = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> PlayerStatePanel = nullptr;

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

	UPROPERTY()
	TMap<ESLInGamePrivateImageType, TObjectPtr<UObject>> PrivateImageMap;

	UPROPERTY()
	TMap<ESLInGameActivateType, bool> ActivateUIMap;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> EffectDynamicMat = nullptr;

	bool bIsObjectiveVisible = false;
	bool bIsExistHitEffect = false;
	int32 CurrentTimeValue = 0;
	FTimerHandle TimerHandle;
};
