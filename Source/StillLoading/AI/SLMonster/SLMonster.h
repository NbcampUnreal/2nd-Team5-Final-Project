// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/SLAIBaseCharacter.h"
#include "SLMonster.generated.h"

UCLASS()
class STILLLOADING_API ASLMonster : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLMonster();

	void Attack();

    UPROPERTY()
    TObjectPtr<AActor> TargetActor;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 200.0f;
	UPROPERTY(EditAnywhere, Category = "AI")

	float AttackCooldown = 0.7f;

	float LastAttackTime;

	/*UFUNCTION(BlueprintCallable, Category = "RVO")
	void SetRVOAvoidanceEnabled(bool bEnable);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceRadius = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceWeight = 0.5f;*/

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
