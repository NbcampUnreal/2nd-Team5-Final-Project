// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLDungeonActivator.h"
#include "SLDungeonTrapFloor.generated.h"

/**
 *
 */
UCLASS()
class STILLLOADING_API ASLDungeonTrapFloor : public ASLDungeonActivator
{
	GENERATED_BODY()

public:

	ASLDungeonTrapFloor();

	virtual void Tick(float DeltaTime) override;

	virtual void Activate() override;

	virtual void DeActivate() override;

	UPROPERTY(EditAnywhere, Category = "ActivateSetting")
	float VisibleTime = 5.0f;


protected:

	virtual void BeginPlay() override;

private:

	void HideMesh();

	void ShowMesh();

	UPROPERTY(VisibleAnywhere, Category = "ActivateSetting")
	float CurrnetVisibleTime;
};
