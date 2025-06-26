// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "SLCheatManager.generated.h"

enum class ESLObjectiveState : uint8;
enum class ESLChapterType : uint8;
/**
 * 
 */
UCLASS()
class STILLLOADING_API USLCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(exec)
	void ShowGridVolumeDebugLine(bool bFlag);

	UFUNCTION(exec)
	void AddCurrentObjectiveProgress();
};
