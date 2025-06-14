// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SLGameStateBase.generated.h"


class USLObjectiveBase;

UCLASS()
class STILLLOADING_API ASLGameStateBase : public AGameState
{
	GENERATED_BODY()

public:
	ASLGameStateBase();

	TArray<TObjectPtr<USLObjectiveBase>>& GetInProgressedObjectives();
	
protected:
	virtual void BeginPlay() override;

private:
	TArray<TObjectPtr<USLObjectiveBase>> InProgressedObjectives;
};
