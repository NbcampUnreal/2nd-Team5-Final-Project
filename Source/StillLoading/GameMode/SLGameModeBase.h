// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SLGameModeBase.generated.h"


class USLObjectiveBase;
class ASLGameStateBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInProgressObjectiveAdded, USLObjectiveBase*, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInProgressObjectiveRemoved, USLObjectiveBase*, Objective);

UCLASS()
class STILLLOADING_API ASLGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	ASLGameModeBase();
	UFUNCTION(BlueprintCallable)
	void AddInProgressObjective(USLObjectiveBase* Objective);
	UFUNCTION(BlueprintCallable)
	void RemoveInProgressObjective(USLObjectiveBase* Objective);
	UFUNCTION(BlueprintCallable)
	USLObjectiveBase* GetPrimaryInProgressObjective();
	
	UPROPERTY(BlueprintAssignable)
	FOnInProgressObjectiveAdded OnInProgressObjectiveAdded;
	UPROPERTY(BlueprintAssignable)
	FOnInProgressObjectiveRemoved OnInProgressObjectiveRemoved;
	
protected:
	virtual void BeginPlay() override;
	virtual void LoadGame();

private:
	void LoadInProgressObjectives();
	
	UPROPERTY()
	TObjectPtr<ASLGameStateBase> SLGameState;
};
