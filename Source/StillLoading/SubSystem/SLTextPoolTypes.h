// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SLTextPoolTypes.generated.h"

UENUM(BlueprintType)
enum class ESLTextDataType : uint8
{
	ETD_None = 0,
	ETD_UI,
	ETD_Story,
	ETD_Talk,
	ETD_Notify,
	ETD_Other
};

UENUM(BlueprintType)
enum class ESLTargetWidgetType : uint8
{
	ETW_None = 0,
	ETW_Title,
	ETW_MapList,
	ETW_InGame,
	ETW_Option,
	ETW_KeySetting,
	ETW_Credit
};

UENUM(BlueprintType)
enum class ESLLanguageType : uint8
{
	EL_None = 0,
	EL_Kor,
	EL_Eng,
	EL_Max
};

UCLASS()
class STILLLOADING_API USLTextPoolTypes : public UObject
{
	GENERATED_BODY()
	
};
