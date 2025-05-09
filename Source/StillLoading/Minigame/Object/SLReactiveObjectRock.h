// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLReactiveObjectRock.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLReactiveObjectRock : public ASLBaseReactiveObject
{
	GENERATED_BODY()
	
protected:
	virtual void OnReacted(const ASLBaseCharacter* InCharacter) override;

public:
	float KnockbackForce = 0.f;
};
