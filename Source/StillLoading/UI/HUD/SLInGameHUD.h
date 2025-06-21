// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUD/SLBaseHUD.h"
#include "SLInGameHUD.generated.h"

class USLInGameWidget;
class USLObjectiveBase;
struct FSLPlayerHpDelegateBuffer;
struct FSLSpecialValueDelegateBuffer;
struct FSLBossHpDelegateBuffer;

UCLASS()
class STILLLOADING_API ASLInGameHUD : public ASLBaseHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ApplyObjective();

	UFUNCTION(BlueprintCallable)
	void ApplyTimer(int32 SecondsValue);

	UFUNCTION(BlueprintCallable)
	void ApplyPlayerHp(FSLPlayerHpDelegateBuffer& PlayerHpDelegate);

	UFUNCTION(BlueprintCallable)
	void ApplyPlayerSpecial(FSLSpecialValueDelegateBuffer& SpecialValueDelegate);

	UFUNCTION(BlueprintCallable)
	void ApplyBossHp(FSLBossHpDelegateBuffer& BossHpDelegate);

	UFUNCTION(BlueprintCallable)
	void ApplyHitEffect(FSLPlayerHpDelegateBuffer& PlayerHpDelegate);


	UFUNCTION(BlueprintCallable)
	bool GetIsActivated(ESLInGameActivateType TargetType);


	UFUNCTION(BlueprintCallable)
	void SetTimerVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	void SetPlayerStateVisibility(bool bIsVisible, bool bIsSpecial);

	UFUNCTION(BlueprintCallable)
	void SetObjectiveVisibility();

	UFUNCTION(BlueprintCallable)
	void SetInvisibleObjectivve();

	UFUNCTION(BlueprintCallable)
	void SetBossStateVisibility(bool bIsVisible);

	// use delegate
	UFUNCTION(BlueprintCallable)
	void SetTimerValue(int32 SecondsValue);

	UFUNCTION(BlueprintCallable)
	void SetPlayerHpValue(float MaxHp, float CurrentHp);

	UFUNCTION(BlueprintCallable)
	void SetPlayerSpecialValue(float MaxValue, float CurrentValue);

	UFUNCTION(BlueprintCallable)
	void SetObjectiveName(const FName& ObjectiveName);

	UFUNCTION(BlueprintCallable)
	void SetObjectiveCounter(const FName& ObjectiveName, int32 MaxCount, int32 CurrentCount);

	UFUNCTION(BlueprintCallable)
	void SetBossHpValue(float MaxHp, float CurrentHp);

	UFUNCTION(BlueprintCallable)
	void SetHitEffectValue(float MaxHp, float CurrentHp);

protected:
	virtual void OnStartedHUD() override;

private:
	UFUNCTION()
	void OnAddObjective(USLObjectiveBase* TargetObjective);

	UFUNCTION()
	void OnRemoveObjective(USLObjectiveBase* TargetObjective);

	UFUNCTION()
	void OnObjectiveCountChanged(int32 Count);

protected:
	UPROPERTY()
	TObjectPtr<USLInGameWidget> InGameWidget = nullptr;

private:
	bool bIsFirstApplyHp = true;

	FName CurrentObjectiveName = "";
	int32 ObjectiveMaxCount = 0;
};
