// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/SLAIBaseCharacter.h"
#include "SLNPC.generated.h"

UCLASS()
class STILLLOADING_API ASLNPC : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLNPC();

	UPROPERTY()
	AActor* TargetActor;

	void Attack();

	

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackCooldown = 2.5f;

	float LastAttackTime;

	UFUNCTION(BlueprintCallable, Category = "RVO")
	void SetRVOAvoidanceEnabled(bool bEnable);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceRadius = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RVO")
	float AvoidanceWeight = 0.5f;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	

};
