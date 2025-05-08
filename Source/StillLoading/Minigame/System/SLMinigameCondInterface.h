// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLMinigameCondInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USLMinigameCondInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STILLLOADING_API ISLMinigameCondInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//조건을 만족하기 위한 변수 및 클리어 여부 초기화
	virtual void InitCondition() = 0;
	//조건이 만족되었는지 체크
	virtual void CheckCondition() = 0;
	//조건이 만족되었다면 결과를 저장해줄 클래스로 전달
	virtual void SendCondition() = 0;
};
