// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SLUITypes.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Objective/SLObjectiveBase.h"
#include "SLSaveDataStructs.generated.h"
/**
 * 
 */

enum class ESLChapterType : uint8;

USTRUCT(BlueprintType)
struct FSettingSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FKey> KeySet;

	UPROPERTY()
	TMap<EInputActionType, FEnhancedActionKeyMapping> ActionKeyMap;

	UPROPERTY()
	ESLLanguageType LanguageType = ESLLanguageType::EL_Kor;

	UPROPERTY()
	float BgmVolume = 1.0f;

	UPROPERTY()
	float EffectVolume = 1.0f;

	UPROPERTY()
	float Brightness = 1.0f;

	UPROPERTY()
	int32 WindowMode = 0;

	UPROPERTY()
	float ScreenWidth = 1920.0f;

	UPROPERTY()
	float ScreenHeight = 1080.0f;
};

USTRUCT(BlueprintType)
struct FObjectiveSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TMap<FName, ESLObjectiveState> ObjectiveSaveDataMap;
};

USTRUCT(BlueprintType)
struct FChapterObjectiveSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TMap<ESLChapterType, FObjectiveSaveData> ChapterObjectiveSaveDataMap;
};

USTRUCT(BlueprintType)
struct FSlotSaveData
{
	GENERATED_BODY()

	FSlotSaveData()
	{
		SaveTime = FDateTime::Now();
	}
	
	UPROPERTY(BlueprintReadWrite, SaveGame)
	int32 SlotIndex = 0;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FDateTime SaveTime;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FName SlotName = TEXT("빈 슬롯");

	UPROPERTY(BlueprintReadWrite, SaveGame)
	float PlayTimeInSeconds = 0.0f;

};