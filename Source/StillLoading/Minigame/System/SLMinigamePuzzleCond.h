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
	void AddNumber(int32 Number);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//조건을 만족하기 위한 변수 및 클리어 여부 초기화
	virtual void InitCondition();

	//조건이 만족되었는지 체크
	virtual void CheckCondition(EResult result);

	//조건이 만족되었다면 결과를 저장해줄 클래스로 전달
	virtual void SendCondition(EResult result);

	//실패했을 경우 모든 석상들을 비활성화
	UFUNCTION()
	void DeactivateStatue();
private:
	UPROPERTY(EditAnywhere)
	TArray<int> CurrentPermutation = {};
	UPROPERTY(VisibleAnywhere)
	TArray<int> AnswerPermutation = {};
};