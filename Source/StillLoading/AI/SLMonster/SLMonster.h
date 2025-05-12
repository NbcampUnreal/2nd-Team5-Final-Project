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

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void Attack();

    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackRange = 130.0f;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackCooldown = 2.5f;

    float LastAttackTime;

    APawn* TargetPlayer;

};
