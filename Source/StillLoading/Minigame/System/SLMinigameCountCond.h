// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StillLoading\Minigame\System\SLBaseMinigameCond.h"
#include "SLMinigameCountCond.generated.h"

UCLASS()
class STILLLOADING_API ASLMinigameCountCond : public ASLBaseMinigameCond
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLMinigameCountCond();

	//횟수가 누적되어야하는 경우 아래 함수를 호출
	//ex) monster가 사망했을 경우 Die함수에서 AddCount함수를 호출
	//    상호작용 횟수를 셈하는 경우 성공을 판단하는 함수에서 AddCount함수를 호출
	void AddCount(int32 InAddCount);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual void InitCondition();

	virtual void SendConditionToMinigameSubsystem(ESLMinigameResult InResult);

private:
	UPROPERTY(VisibleAnywhere)
	int32 Count = 0;
	UPROPERTY(EditAnywhere)
	int32 MaxCount = 0;
};
