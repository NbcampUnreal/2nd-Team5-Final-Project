// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLInGameDelegateBuffers.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHpChanged, float, MaxHp, float, CurrentHp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpecialChanged, float, MaxValue, float, CurrentValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossHpChanged, float, MaxHp, float, CurrentHp);

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLPlayerHpDelegateBuffer
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerHpChanged OnPlayerHpChanged;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLSpecialValueDelegateBuffer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnSpecialChanged OnSpecialValueChanged;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLBossHpDelegateBuffer
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnBossHpChanged OnBossHpChanged;
};
