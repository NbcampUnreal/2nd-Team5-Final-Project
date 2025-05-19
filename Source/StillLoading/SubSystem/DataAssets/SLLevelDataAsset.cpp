// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/DataAssets/SLLevelDataAsset.h"

const TSoftObjectPtr<UWorld> USLLevelDataAsset::GetLevelRef(ESLLevelNameType LevelNameType)
{
	checkf(LevelURLMap.Contains(LevelNameType), TEXT("Level Data Not Contains Level Type"));
	return LevelURLMap[LevelNameType];
}
