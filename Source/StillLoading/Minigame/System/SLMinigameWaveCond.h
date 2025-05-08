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
	void AddCount(int count);

	UFUNCTION()
	void StartNextWave();
protected:
	//조건을 만족하기 위한 변수 및 클리어 여부 초기화
	virtual void InitCondition();

	//조건이 만족되었는지 체크
	virtual void CheckCondition(EResult result);

	//조건이 만족되었다면 결과를 저장해줄 클래스로 전달
	virtual void SendCondition(EResult result);

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
