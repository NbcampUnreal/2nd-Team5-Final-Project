// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SLUITypes.h"
#include "SLTextPoolDataStruct.generated.h"


USTRUCT()
struct STILLLOADING_API FSLTextPoolDataStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	ESLLanguageType LanguageType = ESLLanguageType::EL_Kor;

	UPROPERTY()
	TSoftObjectPtr<UDataTable> TextDataTable = nullptr;
};
