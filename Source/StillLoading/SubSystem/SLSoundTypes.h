// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SLSoundTypes.generated.h"

UENUM(BlueprintType)
enum class ESLBgmSoundType : uint8
{
	EBS_None = 0,
	EBS_Intro,
	EBS_Title,
	EBS_Battle,
	EBS_Mini1,
	EBS_Mini2,
	EBS_Mini3,
	EBS_Mini4,
	EBS_Mini5,
	EBS_DebugRoom,
	EBS_BossBattle,
	EBS_Ending
};

UENUM(BlueprintType)
enum class ESLUISoundType : uint8
{
	EUS_None = 0,
	EUS_Click,
	EUS_Hover,
	EUS_Open,
	EUS_Close,
	EUS_Notify,
	EUS_Talk
};

UCLASS()
class STILLLOADING_API USLSoundTypes : public UObject
{
	GENERATED_BODY()
	
};
