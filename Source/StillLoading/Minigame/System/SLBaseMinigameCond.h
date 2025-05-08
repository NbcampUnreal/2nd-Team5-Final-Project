// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLBaseMinigameCond.generated.h"

UCLASS()
class STILLLOADING_API ASLBaseMinigameCond : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLBaseMinigameCond();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//조건을 만족하기 위한 변수 및 클리어 여부 초기화
	virtual void InitCondition();
	//조건이 만족되었는지 체크
	virtual void CheckCondition();
	//조건이 만족되었다면 결과를 저장해줄 클래스로 전달
	virtual void SendCondition();

protected:
	bool bIsClear = false;


};
