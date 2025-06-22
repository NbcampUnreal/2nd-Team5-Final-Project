// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLObjectiveDataAsset.h"
#include "SLObjectiveSubsystem.generated.h"

/**
 * 
 */

enum class ESLObjectiveState : uint8;
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

	UFUNCTION(BlueprintCallable)
	ESLObjectiveState GetObjectiveState(ESLChapterType Chapter, FName Name);

	UFUNCTION(BlueprintCallable)
	void SetObjectiveState(ESLChapterType Chapter, FName Name, ESLObjectiveState State);

	UFUNCTION(BlueprintCallable)
	void SetObjectiveStateDelayed(ESLChapterType Chapter, FName Name, ESLObjectiveState State);
	
	UFUNCTION(BlueprintCallable)
	TArray<USLObjectiveBase*> GetInProgressedObjectives();

	TMap<ESLChapterType, FSLObjectiveRuntimeData>& GetCachedObjectiveDataRef();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
private:
	UPROPERTY()
	TMap<ESLChapterType, FSLObjectiveRuntimeData> CachedObjectiveData;
};
