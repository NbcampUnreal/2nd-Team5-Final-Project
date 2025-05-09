// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StillLoading\Minigame\System\SLBaseMinigameCond.h"
#include "SLMinigamePuzzleCond.generated.h"

UCLASS()
class STILLLOADING_API ASLMinigamePuzzleCond : public ASLBaseMinigameCond
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLMinigamePuzzleCond();

	//석상이 활성화되었을 때 해당 석상의 고유 번호를 아래 함수로 전달
	void AddNumber(int32 InNumber);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void DeactivateAllStatue();

	virtual void InitCondition();

	virtual void SendCondition(ESLMinigameResult InResult);
private:
	//초기화는 생성자
	UPROPERTY(EditAnywhere)
	TArray<int> CurrentPermutation;
	UPROPERTY(VisibleAnywhere)
	TArray<int> AnswerPermutation;
};