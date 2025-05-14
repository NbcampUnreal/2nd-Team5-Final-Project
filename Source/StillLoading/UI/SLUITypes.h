// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SLUITypes.generated.h"

UENUM(BlueprintType)
enum class ESLAdditiveWidgetType : uint8
{
	EAW_OptionWidget = 0,
	EAW_FadeWidget,
	EAW_StoryWidget,
	EAW_TalkWidget,
	EAW_NotifyWidget,
	EAW_CreditWidget,
	EAW_KeySettingWidget
};

UENUM(BlueprintType)
enum class ESLUISoundType : uint8
{
	EUS_None = 0,
	EUS_Click,
	EUS_Hover,
	EUS_Open,
	EUS_Close,
	EUS_Notify
};

UENUM(BlueprintType)
enum class ESLChapterType : uint8
{
	EC_Intro = 0,
	EC_Chapter2D,
	EC_Chapter2_5D,
	EC_Chapter3D,
	EC_ChapterHighQuality
};

UENUM(BlueprintType)
enum class ESLInputModeType : uint8
{
	EIM_UIOnly = 0,
	EIM_GameOnly,
	EIM_GameAndUI
};

UENUM(BlueprintType)
enum class ESLGameMapType : uint8
{
	EGM_None = 0,
	EGM_Mini1,
	EGM_Mini2,
	EGM_Mini3,
	EGM_Mini4,
	EGM_Mini5,
	EGM_DebugRoom,
	EGM_Boss,
	EGM_Max
};

UCLASS()
class STILLLOADING_API USLUITypes : public UObject
{
	GENERATED_BODY()
	
};
