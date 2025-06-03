// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLObjectiveHandlerBase.generated.h"


enum class ESLObjectiveState : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLObjectiveHandlerBase : public UActorComponent
{
	GENERATED_BODY()

public:
	USLObjectiveHandlerBase();

public:
	UFUNCTION()
	virtual void OnObjectiveStateChanged(ESLObjectiveState ObjectiveState);

	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ObjectiveList;
	
private:
	void SubscribeObjectives();
};
