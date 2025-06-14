// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLGameModeBase.h"
#include "GameFramework/GameMode.h"
#include "SLGameMode.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLGameMode : public ASLGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
};
