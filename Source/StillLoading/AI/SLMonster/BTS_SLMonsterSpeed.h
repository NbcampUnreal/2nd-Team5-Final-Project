// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_SLMonsterSpeed.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API UBTS_SLMonsterSpeed : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTS_SLMonsterSpeed();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DelatSeconds) override;

	UPROPERTY(EditAnywhere, Category ="AI")
	float AttackRange = 150.f;
	UPROPERTY(EditAnywhere, Category ="AI")
	float AttackAngle = 60.f;
	UPROPERTY(EditAnywhere, Category = "AI")
	float CheckRadius = 400.f;
};
