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

	// 설정 정보
	UPROPERTY()
	FSettingSaveData UserSaveData;

	// 실제 게임 진행상황(오브젝티브)
	UPROPERTY()
	FChapterObjectiveSaveData ObjectiveSaveData;

	// 세이브 슬롯에 보여질 정보
	UPROPERTY()
	FSlotSaveData SlotSaveData;
};
