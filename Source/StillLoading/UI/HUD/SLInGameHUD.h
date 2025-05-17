// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUD/SLBaseHUD.h"
#include "SLInGameHUD.generated.h"

class USLInGameWidget;

UCLASS()
class STILLLOADING_API ASLInGameHUD : public ASLBaseHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetTimerVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	void SetPlayerStateVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	void SetGameStateVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	void SetHitEffectVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	void SetBossStateVisibility(bool bIsVisible);

	// use delegate
	UFUNCTION(BlueprintCallable)
	void SetTimerValue(int32 SecondsValue);

	UFUNCTION(BlueprintCallable)
	void SetPlayerHpValue(int32 MaxHp, int32 CurrentHp);

	UFUNCTION(BlueprintCallable)
	void SetGameStateValue(int32 TargetState, const FText& NewText); // use enum

	UFUNCTION(BlueprintCallable)
	void SetBossHpValue(int32 MaxHp, int32 CurrentHp);

protected:
	virtual void OnStartedHUD() override;

protected:
	UPROPERTY()
	TObjectPtr<USLInGameWidget> InGameWidget = nullptr;
};
