// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SLSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	USLSaveGame();

	UPROPERTY()
	int CurrentLevel;

	UPROPERTY()
	int CurrentChapter;

};
