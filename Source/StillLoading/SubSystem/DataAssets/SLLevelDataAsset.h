// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLLevelDataAsset.generated.h"

UCLASS()
class STILLLOADING_API USLLevelDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const TSoftObjectPtr<UWorld> GetLevelRef(ESLLevelNameType LevelNameType);

private:
	UPROPERTY(EditAnywhere)
	TMap<ESLLevelNameType, TSoftObjectPtr<UWorld>> LevelURLMap;
};
