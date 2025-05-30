// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SLGameStateBase.generated.h"

class USLGameSubjectBase;
/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLGameStateBase : public AGameState
{
	GENERATED_BODY()

public:
	ASLGameStateBase();
	
	TArray<USLGameSubjectBase*> GetGameSubjects() const;
	void AddGameSubject(USLGameSubjectBase *Subject);
	void RemoveGameSubject(USLGameSubjectBase *Subject);
	
private:
	UPROPERTY()
	TArray<TObjectPtr<USLGameSubjectBase>> GameSubjects;
};
