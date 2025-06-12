// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLDungeonObjectInterface.generated.h"

UINTERFACE(MinimalAPI)
class USLDungeonObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class STILLLOADING_API ISLDungeonObjectInterface
{
	GENERATED_BODY()

public:

	virtual void Activate() = 0;

	virtual void DeActivate() = 0;
};
