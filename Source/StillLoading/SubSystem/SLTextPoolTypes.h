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

UCLASS()
class STILLLOADING_API USLTextPoolTypes : public UObject
{
	GENERATED_BODY()
	
};
