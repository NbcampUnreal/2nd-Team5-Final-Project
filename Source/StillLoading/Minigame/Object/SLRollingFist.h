// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLDungeonActivator.h"
#include "SLRollingFist.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLRollingFist : public ASLDungeonActivator
{
	GENERATED_BODY()

public:
	ASLRollingFist();

	virtual void Tick(float DeltaTime) override;

	virtual void Activate() override;

	virtual void DeActivate() override;

	UPROPERTY(EditAnywhere, Category = "ActivateSetting")
	FVector ReturnPosition;

	UPROPERTY(EditAnywhere, Category = "ActivateSetting")
	FVector TargetPosition;

	UPROPERTY(EditAnywhere, Category = "ActivateSetting")
	float Speed = 100.f;


protected:

	virtual void BeginPlay() override;


private:

	UPROPERTY()
	bool bIsTrigger = false;

	UPROPERTY()
	bool bArrivalTargetPoint = false;

	UPROPERTY()
	float RotationSpeed;

};
