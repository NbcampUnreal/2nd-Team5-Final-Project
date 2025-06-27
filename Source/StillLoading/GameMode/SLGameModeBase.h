// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Objective/SLObjectiveBase.h"
#include "SLGameModeBase.generated.h"

class ASLGameStateBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInProgressObjectiveAdded, USLObjectiveBase*, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInProgressObjectiveRemoved, USLObjectiveBase*, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrimaryInProgressObjectiveChanged, USLObjectiveBase*, Objective);

UCLASS()
class STILLLOADING_API ASLGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	ASLGameModeBase();
	UFUNCTION(BlueprintCallable)
	void OnUpdatedObjectiveState(USLObjectiveBase* Objective, const ESLObjectiveState InState);
	UFUNCTION(BlueprintCallable)
	void AddInProgressObjective(USLObjectiveBase* Objective);
	UFUNCTION(BlueprintCallable)
	void RemoveInProgressObjective(USLObjectiveBase* Objective);
	UFUNCTION(BlueprintCallable)
	USLObjectiveBase* GetPrimaryInProgressObjective();
	UFUNCTION(BlueprintCallable)
	void ResetModifiedObjectives();
	
	UPROPERTY(BlueprintAssignable)
	FOnInProgressObjectiveAdded OnInProgressObjectiveAdded;
	UPROPERTY(BlueprintAssignable)
	FOnInProgressObjectiveRemoved OnInProgressObjectiveRemoved;
	UPROPERTY(BlueprintAssignable)
	FOnPrimaryInProgressObjectiveChanged OnPrimaryInProgressObjectiveChanged;
	
protected:
	virtual void BeginPlay() override;
	virtual void LoadGame();

private:
	void LoadInProgressObjectives();
	
	UPROPERTY()
	TObjectPtr<ASLGameStateBase> SLGameState;
};
