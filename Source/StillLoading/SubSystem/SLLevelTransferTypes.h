// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLLevelTransferTypes.generated.h"

UENUM(BlueprintType)
enum class ESLLevelNameType : uint8
{
	ELN_None = 0,
	ELN_Intro,
	ELN_MainTitle,
	ELN_Title,
	ELN_MapList,
	ELN_Mini1,
	ELN_Mini2,
	ELN_Mini3,
	ELN_Mini4,
	ELN_Mini5,
	ELN_Mini6,
	ELN_DebugRoom,
	ELN_BossStage,
	ELN_Ending
};

UENUM(BlueprintType)
enum class ESLChapterType : uint8
{
	EC_None = 0 UMETA(DisplayName = "---"),
	EC_Chapter0 UMETA(DisplayName = "Intro"),
	EC_Chapter1 UMETA(DisplayName = "Chapter 1"),
	EC_Chapter2 UMETA(DisplayName = "Chapter 2"),
	EC_Chapter3 UMETA(DisplayName = "Chapter 3"),
	EC_Chapter4 UMETA(DisplayName = "Chapter 4"),
	EC_Main UMETA(DisplayName = "Main Title")
};

UCLASS()
class STILLLOADING_API USLLevelTransferTypes : public UObject
{
	GENERATED_BODY()
	
};
