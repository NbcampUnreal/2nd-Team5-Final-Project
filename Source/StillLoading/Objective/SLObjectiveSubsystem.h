// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLObjectiveSubsystem.generated.h"

/**
 * 
 */

enum class ESLChapterType : uint8;
class USLObjectiveDataAsset;
class USLObjectiveDataSettings;
class USLObjectiveBase;

UCLASS()
class STILLLOADING_API USLObjectiveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	USLObjectiveBase* GetObjective(ESLChapterType Chapter, FName Name);

private:
	UPROPERTY()
	const USLObjectiveDataSettings* ObjectiveDataSettings = nullptr;
};
