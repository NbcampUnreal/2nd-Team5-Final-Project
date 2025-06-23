// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "SLGeometryCollectionActor.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLGeometryCollectionActor : public AGeometryCollectionActor
{
	GENERATED_BODY()


public:
	ASLGeometryCollectionActor();

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//void ScheduleDebrisCleanup(float DelaySeconds = 1.0f);

	//void RequestDebrisCleanup();

private:

	/*UPROPERTY()
	FTimerHandle CleanupTimerHandle;*/


};
