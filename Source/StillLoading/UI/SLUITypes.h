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
enum class ESLLanguageType : uint8
{
	EL_Kor = 0,
	EL_Eng
};

UCLASS()
class STILLLOADING_API USLUITypes : public UObject
{
	GENERATED_BODY()
	
};
