// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "SLTextPoolDataStruct.generated.h"

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLTextPoolDataStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TMap<ESLLanguageType, TSoftObjectPtr<UDataTable>> LanguageTextTableMap;
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLTextTypeStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<ESLTextDataType, FSLTextPoolDataStruct> TextTypeMap;
};