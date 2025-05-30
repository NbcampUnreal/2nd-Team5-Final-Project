// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameSubject/SLGameSubjectBase.h"
#include "SLGameModeBase.generated.h"


class ASLGameStateBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSubjectStarted, USLGameSubjectBase*, GameSubject)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSubjectFinished, USLGameSubjectBase*, GameSubject)

UCLASS()
class STILLLOADING_API ASLGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	ASLGameModeBase();
	virtual void StartGameSubject(USLGameSubjectBase *Subject);
	virtual void EndGameSubject(USLGameSubjectBase *Subject);
	
protected:
	virtual void BeginPlay() override;
	virtual void LoadGame();
	
public:
	FOnGameSubjectStarted OnGameSubjectStartedDelegate;
	FOnGameSubjectFinished OnGameSubjectFinishedDelegate;

private:
	TSoftObjectPtr<ASLGameStateBase> SLGameState;
};
