// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLGeometryManagerSubsystem.generated.h"


class AGeometryCollectionActor;

UCLASS()
class STILLLOADING_API USLGeometryManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void RegisterGeometryCollection(AGeometryCollectionActor* GCActor);

	UFUNCTION()
	void UnregisterGeometryCollection(AGeometryCollectionActor* GCActor);

	//파편 정지
	UFUNCTION(BlueprintCallable)
	void StopAllGeometryChunks();

protected:

	UPROPERTY()
	TArray<TWeakObjectPtr<AGeometryCollectionActor>> RegisteredGCActors;

};
