// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SLSaveDataStructs.h"
#include "Objective/SLObjectiveDataAsset.h"
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

	//저장할 데이터 변수들 추가하세요
	UPROPERTY(EditAnywhere, BlueprintReadWrite) //테스트용 임시 변수
	int CurrentMiniGameLevel;

	UPROPERTY()
	ESLChapterType CurrentChapter = ESLChapterType::EC_Chapter0;

	UPROPERTY()
	FUserSaveData UserSaveData;

	// UPROPERTY()
	// TMap<ESLChapterType, FSLObjectiveRuntimeData> ObjectiveSaveData;

	UPROPERTY()
	FChapterObjectiveSaveData ObjectiveSaveData;
	
};
