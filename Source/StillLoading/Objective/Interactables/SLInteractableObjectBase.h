// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLInteractableObjectBase.generated.h"

UINTERFACE()
class USLInteractableObjectBase : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STILLLOADING_API ISLInteractableObjectBase
{
	GENERATED_BODY()

public:
	virtual void Interaction() = 0;
};
