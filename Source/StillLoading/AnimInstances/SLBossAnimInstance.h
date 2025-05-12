// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLAICharacterAnimInstance.h"
#include "SLBossAnimInstance.generated.h"

UCLASS()
class STILLLOADING_API USLBossAnimInstance : public USLAICharacterAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	EBossAttackPattern BossAttackPattern;
};
