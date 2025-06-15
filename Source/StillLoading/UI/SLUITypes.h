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
	EAW_CreditWidget
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
	EGM_Mini6,
	EGM_DebugRoom,
	EGM_Boss,
	EGM_Max
};

UENUM(BlueprintType)
enum class ESLNotifyType : uint8
{
	EN_None = 0,
	EN_Find,
	EN_Spawn,
	EN_Info,
	EN_Encounter
};

UENUM(BlueprintType)
enum class ESLTalkTargetType : uint8
{
	ETT_None = 0,
	ETT_Hero,
	ETT_Heroine,
	ETT_Boss,
	ETT_NPC,
	ETT_Object
};

UENUM(BlueprintType)
enum class ESLStoryType : uint8
{
	ES_Start = 0,
	ES_End
};

UENUM(BlueprintType)
enum class ESLPublicWidgetImageType : uint8
{
	EPWI_None = 0,
	EPWI_NormalBorder,
	EPWI_NameBorder,
	EPWI_TextBorder,
	EPWI_Button,
	EPWI_ButtonHover,
	EPWI_ButtonEffect,
	EPWI_ButtonRetainer,
	EPWI_Background,
	EPWI_Fade
};

UENUM(BlueprintType)
enum class ESLInGamePrivateImageType : uint8
{
	EGPI_None = 0,
	EGPI_HitEffect,
	EGPI_PlayerHpBack,
	EGPI_PlayerHpBar,
	EGPI_SpecailBack,
	EGPI_SpecialBar,
	EGPI_BossHpBack,
	EGPI_BossHpBar,
	EGPI_TimerBorder,
	EGPI_TimerHands
};

UENUM(BlueprintType)
enum class ESLTitlePrivateImageType : uint8
{
	ETPI_None = 0,
	ETPI_Background,
	ETPI_Logo
};

UENUM(BlueprintType)
enum class ESLOptionPrivateImageType : uint8
{
	EOPI_None = 0,
	EOPI_SliderBack,
	EOPI_SliderBar,
	EOPI_Expandable,
	EOPI_ExpandedButton,
	EOPI_ListBack,
	EOPI_LeftArrow,
	EOPI_RightArrow,
	EOPI_Background
};

UCLASS()
class STILLLOADING_API USLUITypes : public UObject
{
	GENERATED_BODY()
	
};
