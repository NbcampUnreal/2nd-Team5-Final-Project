// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleComponent.h"
#include "SL2DBattleComponenet.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USL2DBattleComponenet : public UBattleComponent
{
	GENERATED_BODY()

public:
	virtual void DoAttackSweep(EAttackAnimType AttackType) override;
	virtual bool DoSweep(EAttackAnimType AttackType) override;

private:
	UPROPERTY(EditAnywhere)
	float AttackSize = 50.f;
	UPROPERTY(EditAnywhere)
	float AttackOffset = 100.f;
};
