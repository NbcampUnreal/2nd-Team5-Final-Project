// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLDemoSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResetInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIncreaseCoin, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIncreaseTime, int32, Sec);

UCLASS()
class STILLLOADING_API USLDemoSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void StartNewGame();

	UFUNCTION(BlueprintCallable)
	void EndCurrentGame();

	UFUNCTION(BlueprintCallable)
	void IncreaseCoin();

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentCoin();

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentTime();

	UFUNCTION(BlueprintCallable)
	void PauseTimer(bool bIsCinematic = false);

	UFUNCTION(BlueprintCallable)
	void ContinueTimer();

	UFUNCTION(BlueprintCallable)
	bool GetIsCinePause() const;

	UFUNCTION(BlueprintCallable)
	void UpdateLastInfo();

	UFUNCTION(BlueprintCallable)
	void ResetCurrentInfo();
	
private:
	UFUNCTION()
	void IncreaseTime();

	void ResetDemoInfo();

public:
	UPROPERTY(BlueprintAssignable)
	FResetInfo ResetDele;

	UPROPERTY(BlueprintAssignable)
	FIncreaseCoin CoinDele;

	UPROPERTY(BlueprintAssignable)
	FIncreaseTime TimeDele;

private:
	int32 CoinCount = 0;
	int32 TimeSec = 0;
	int32 LastCoin = 0;
	int32 LastTime = 0;

	FTimerHandle PlayTimer;
	bool bIsCine = false;
};
