// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SubSystem/SLLevelTransferTypes.h"
#include "SLObjectiveHandlerBase.generated.h"


class USLObjectiveBase;
enum class ESLChapterType : uint8;
enum class ESLObjectiveState : uint8;

class USLObjectiveHandlerBase;
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnObjectiveCompletedSignature, USLObjectiveHandlerBase, OnObjectiveCompletedDelegate, USLObjectiveHandlerBase*, Component);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnObjectiveInProgressedSignature, USLObjectiveHandlerBase, OnObjectiveInProgressedDelegate, USLObjectiveHandlerBase*, Component);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnObjectiveFailedSignature, USLObjectiveHandlerBase, OnObjectiveFailedDelegate, USLObjectiveHandlerBase*, Component);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnObjectiveStandbySignature, USLObjectiveHandlerBase, OnObjectiveStandbyDelegate, USLObjectiveHandlerBase*, Component);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnObjectiveProgressUpdatedSignature, USLObjectiveHandlerBase, OnObjectiveProgressUpdatedDelegate, int32, ProgressCount);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLObjectiveHandlerBase : public UActorComponent
{
	GENERATED_BODY()

public:
	USLObjectiveHandlerBase();
	
	UFUNCTION(BlueprintCallable, Category = "ObjectiveHandler")
	USLObjectiveBase* GetObjective();

	UFUNCTION(BlueprintCallable, Category = "ObjectiveHandler")
	void SetObjectiveState(const ESLObjectiveState State);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "ObjectiveHandler")
	void OnObjectiveCompleted();
	virtual void OnObjectiveCompleted_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "ObjectiveHandler")
	void OnObjectiveInProgressed();
	virtual void OnObjectiveInProgressed_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "ObjectiveHandler")
	void OnObjectiveFailed();
	virtual void OnObjectiveFailed_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "ObjectiveHandler")
	void OnObjectiveStandby();
	virtual void OnObjectiveStandby_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "ObjectiveHandler")
	void OnObjectiveProgressUpdated(const int32 ProgressCount);
	virtual void OnObjectiveProgressUpdated_Implementation(const int32 ProgressCount);
	
private:
	UFUNCTION()
	void OnObjectiveStateChanged(const ESLObjectiveState ObjectiveState);

	void InitializeObjective();

public:
	UPROPERTY(BlueprintAssignable, Category="ObjectiveHandler")
	FOnObjectiveCompletedSignature OnObjectiveCompletedDelegate;
	UPROPERTY(BlueprintAssignable, Category="ObjectiveHandler")
	FOnObjectiveInProgressedSignature OnObjectiveInProgressedDelegate;
	UPROPERTY(BlueprintAssignable, Category="ObjectiveHandler")
	FOnObjectiveFailedSignature OnObjectiveFailedDelegate;
	UPROPERTY(BlueprintAssignable, Category="ObjectiveHandler")
	FOnObjectiveStandbySignature OnObjectiveStandbyDelegate;
	UPROPERTY(BlueprintAssignable, Category="ObjectiveHandler")
	FOnObjectiveProgressUpdatedSignature OnObjectiveProgressUpdatedDelegate;
	
protected:
	UPROPERTY(EditAnywhere, Category = "ObjectiveHandler")
	ESLChapterType Chapter = ESLChapterType::EC_None;
	UPROPERTY(EditAnywhere, Category = "ObjectiveHandler")
	FName ObjectiveName;
};
