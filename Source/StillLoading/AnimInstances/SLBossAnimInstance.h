// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLAICharacterAnimInstance.h"
#include "Character/SLBossCharacter.h"
#include "SLBossAnimInstance.generated.h"

UCLASS()
class STILLLOADING_API USLBossAnimInstance : public USLAICharacterAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetBossAttackPattern(EBossAttackPattern NewBossAttackPattern);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	EBossAttackPattern GetBossAttackPattern();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EBossAttackPattern BossAttackPattern;
};
