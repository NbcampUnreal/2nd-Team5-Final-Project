// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/System/SLBaseMinigameCond.h"
#include "SLMinigameWaveCond.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLMinigameWaveCond : public ASLBaseMinigameCond
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void AddCount(int32 InCount);

	UFUNCTION()
	void StartNextWave();
protected:
	virtual void InitCondition();

	virtual void SendCondition(ESLMinigameResult InResult);

private:
	//킬 수를 카운트할 변수
	UPROPERTY(VisibleAnywhere)
	int32 CurrentKillCount = 0;
	UPROPERTY(EditAnywhere)
	int32 MaxKillCount = 0;

	//웨이브를 카운트할 변수
	UPROPERTY(VisibleAnywhere)
	int32 CurrentWave = 0;
	UPROPERTY(EditAnywhere)
	int32 MaxWave = 0;
};
