// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StillLoading\Minigame\System\SLBaseMinigameCond.h"
#include "SLMinigamePuzzleCond.generated.h"




UENUM(BlueprintType)
enum class ESLPuzzleType : uint8
{
	RotatePuzzle,
	LightPuzzle
};

class ASLInteractableObjectStatue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMinigameCompleted);

UCLASS()
class STILLLOADING_API ASLMinigamePuzzleCond : public ASLBaseMinigameCond
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLMinigamePuzzleCond();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMinigameObjectResetRequested);

	UPROPERTY(BlueprintAssignable)
	FOnMinigameObjectResetRequested ObjectResetRequested;

	UPROPERTY(BlueprintAssignable)
	FOnMinigameCompleted OnMinigameCompleted;
	
	//석상이 활성화되었을 때 해당 석상의 고유 번호를 아래 함수로 전달
	void UpdateStatueState(int8 InStatueIndex, int8 SubmittedValue);

	void SubmittedAnswer();

	void RegisterStatue(ASLInteractableObjectStatue* InStatue);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	ESLPuzzleType PuzzleType;

	

	UPROPERTY(VisibleAnywhere)
	int32 TryCount = 0;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void DeactivateAllStatue();

	UFUNCTION()
	void ResetCondition();

	virtual void InitCondition();

	virtual void SendCondition(ESLMinigameResult InResult);
private:
	//배열의 인덱스는 각 석상을 가르킴. 값은 석상의 상태를 의미
	//디버그룸(1)에서 작동 예시: CurrentStates[4] = 2 = 4번 석상이 2번째 방향을 가르키는 중
	//디버그룸(2)에서 작동 예시: CurrentStates[1] = 4 = 1번 석상이 4번째로 활성화됨.
	//, meta = (EditCondition = "PuzzleType == ESLPuzzleType::RotatePuzzle", EditConditionHides)
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	TArray<int> CurrentStates;
	UPROPERTY(EditAnywhere, Category = "Puzzle")
	TArray<int> AnswerStates;

	UPROPERTY(EditAnywhere)
	bool GameSucceedFlag = false;


	


	TArray<TObjectPtr<ASLInteractableObjectStatue>> Statues;
};