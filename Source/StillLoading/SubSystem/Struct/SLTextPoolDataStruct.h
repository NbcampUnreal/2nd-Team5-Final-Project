// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/SLTextPoolTypes.h"
#include "SLTextPoolDataStruct.generated.h"


USTRUCT()
struct STILLLOADING_API FSLTextPoolDataStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TMap<ESLLanguageType, TSoftObjectPtr<UDataTable>> LanguageTextTableMap;
};
