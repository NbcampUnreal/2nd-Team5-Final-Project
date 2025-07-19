// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SLSaveDataStructs.h"
#include "SubSystem/SLLevelTransferTypes.h"
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
	ESLChapterType CurrentChapterSaveData = ESLChapterType::EC_Chapter0;

	UPROPERTY()
	ESLLevelNameType CurrentLevelSaveData = ESLLevelNameType::ELN_Title;
	
	UPROPERTY()
	FUserSaveData UserSaveData;

	// UPROPERTY()
	// TMap<ESLChapterType, FSLObjectiveRuntimeData> ObjectiveSaveData;

	UPROPERTY()
	FChapterObjectiveSaveData ObjectiveSaveData;

};
