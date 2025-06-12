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
	void ApplyObjective(const FName& ObjectiveName);

	UFUNCTION(BlueprintCallable) // Need Delegate
	void ApplyObjectiveByCounter(const FName& ObjectiveName, int32 MaxCount);

	UFUNCTION(BlueprintCallable) // Need Delegate
	void ApplyTimer(int32 SecondsValue);

	UFUNCTION(BlueprintCallable) // Need Delegate
	void ApplyPlayerHp(int32 MaxHp);

	UFUNCTION(BlueprintCallable) // Need Delegate
	void ApplyPlayerSpecial(int32 MaxValue);

	UFUNCTION(BlueprintCallable) // Need Delegate
	void ApplyBossHp(int32 MaxHp);

	UFUNCTION(BlueprintCallable)
	void ApplyHitEffect();


	UFUNCTION(BlueprintCallable)
	void SetTimerVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	void SetPlayerStateVisibility(bool bIsVisible, bool bIsSpecial);

	UFUNCTION(BlueprintCallable)
	void SetObjectiveVisibility();

	UFUNCTION(BlueprintCallable)
	void SetBossStateVisibility(bool bIsVisible);

	// use delegate
	UFUNCTION(BlueprintCallable)
	void SetTimerValue(int32 SecondsValue);

	UFUNCTION(BlueprintCallable)
	void SetPlayerHpValue(int32 MaxHp, int32 CurrentHp);

	UFUNCTION(BlueprintCallable)
	void SetPlayerSpecialValue(int32 MaxValue, int32 CurrentValue);

	UFUNCTION(BlueprintCallable)
	void SetObjectiveName(const FName& ObjectiveName);

	UFUNCTION(BlueprintCallable)
	void SetObjectiveCounter(const FName& ObjectiveName, int32 MaxCount, int32 CurrentCount);

	UFUNCTION(BlueprintCallable)
	void SetBossHpValue(int32 MaxHp, int32 CurrentHp);

protected:
	virtual void OnStartedHUD() override;

protected:
	UPROPERTY()
	TObjectPtr<USLInGameWidget> InGameWidget = nullptr;
};
