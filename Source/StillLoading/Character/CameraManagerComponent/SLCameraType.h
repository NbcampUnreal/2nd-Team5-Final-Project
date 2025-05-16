// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SLCameraType.generated.h"

UENUM(BlueprintType)
enum class EGameCameraType : uint8
{
	EGCT_ThirdPerson UMETA(DisplayName = "ThirdPerson"),
	EGCT_FirstPerson UMETA(DisplayName = "FirstPerson"),
	EGCT_TopDown UMETA(DisplayName = "TopDown"),
	EGCT_Cinematic UMETA(DisplayName = "Cinematic"),
	EGCT_SideView UMETA(DisplayName = "SideView"),

	EGCT_MAX UMETA(Hidden) // 범위 체크용
};
