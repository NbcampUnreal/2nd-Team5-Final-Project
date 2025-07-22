// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLSaveDataStructs.h"
#include "GameFramework/SaveGame.h"
#include "SLSettingSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLSettingSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	// 설정 정보
	UPROPERTY()
	FSettingSaveData SettingSaveData;
};
