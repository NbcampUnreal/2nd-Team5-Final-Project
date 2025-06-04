// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SLGameModeBase.generated.h"


class ASLGameStateBase;

UCLASS()
class STILLLOADING_API ASLGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	ASLGameModeBase();
	
protected:
	virtual void BeginPlay() override;
	virtual void LoadGame();

private:
	TSoftObjectPtr<ASLGameStateBase> SLGameState;
};
