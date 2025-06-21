// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLObjectiveBase.generated.h"

struct FObjectiveSaveData;

UENUM(BlueprintType)
enum class ESLObjectiveState : uint8
{
	None = 0,
	Standby,
	Complete,
	Fail,
	InProgress
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveStateChanged, ESLObjectiveState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveProgressChanged, int32, Count);

UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "게임 목표")
class USLObjectiveBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void AddObjectiveProgress(int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void ObjectiveFail();

	UFUNCTION(BlueprintCallable, Category = "Objective")
	FName GetObjectiveName() const { return ObjectiveName; }
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void SetName(const FName& InName) { ObjectiveName = InName; }

	UFUNCTION(BlueprintCallable, Category = "Objective")
	FString GetDescription() const { return Description; }
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void SetDescription(const FString& InDescription) { Description = InDescription; }

	UFUNCTION(BlueprintCallable, Category = "Objective")
	ESLObjectiveState GetObjectiveState() const { return ObjectiveState; }
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void SetObjectiveState(const ESLObjectiveState InState);
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void SetObjectiveStateDelayed(const ESLObjectiveState InState, const float DelayTime = 0.1f);

	UFUNCTION(BlueprintCallable, Category = "Objective")
	int32 GetObjectiveProgressCount() const { return ObjectiveProgressCount; }

	UFUNCTION(BlueprintCallable, Category = "Objective")
	int32 GetObjectiveCompleteCount() const { return ObjectiveCompleteCount; }

public:
	UPROPERTY(BlueprintAssignable, Category = "Objective Delegate")
	FOnObjectiveStateChanged OnObjectiveStateChanged;
	UPROPERTY(BlueprintAssignable, Category = "Objective Delegate")
	FOnObjectiveProgressChanged OnObjectiveProgressChanged;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Objective")
	FName ObjectiveName;
	UPROPERTY(EditAnywhere, Category = "Objective")
	FString Description;
	UPROPERTY(EditAnywhere, Category = "Objective")
	ESLObjectiveState ObjectiveState = ESLObjectiveState::None;
	UPROPERTY(EditAnywhere, Category = "Objective")
	int32 ObjectiveProgressCount = 0;
	UPROPERTY(EditAnywhere, Category = "Objective")
	int32 ObjectiveCompleteCount = 1;
};