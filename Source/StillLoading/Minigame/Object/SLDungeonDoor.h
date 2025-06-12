// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLDungeonActivator.h"
#include "SLDungeonDoor.generated.h"

/**
 *
 */
UCLASS()
class STILLLOADING_API ASLDungeonDoor : public ASLDungeonActivator
{
	GENERATED_BODY()

public:
	ASLDungeonDoor();

	virtual void Tick(float DeltaTime) override;

	virtual void Activate() override;

	virtual void DeActivate() override;

	UPROPERTY(EditAnywhere, Category = "Door")
	FVector OpenPosition;

	UPROPERTY(EditAnywhere, Category = "Door")
	float SlideSpeed = 100.f;


protected:

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FVector ClosedPosition;

	UPROPERTY()
	bool bIsTrigger = false;

	UPROPERTY()
	bool bIsOpen = false;


};
