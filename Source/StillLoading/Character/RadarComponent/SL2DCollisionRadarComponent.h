// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionRadarComponent.h"
#include "SL2DCollisionRadarComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USL2DCollisionRadarComponent : public UCollisionRadarComponent
{
	GENERATED_BODY()

public:
	USL2DCollisionRadarComponent();

protected:
	virtual const FVector GetForwardVector() override;
};
