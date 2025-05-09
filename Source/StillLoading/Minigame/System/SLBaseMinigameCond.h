// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Minigame/System/SLMinigameCondSubsystem.h"
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

	//미니게임이 실패했음을 감지하는 객체에서 아래 함수를 호출하여 미니게임의 결과 전달.
	UFUNCTION()
	virtual void MinigameFailed();
	//미니게임이 패스되었음을 감지하는 객체에서 아래 함수를 호출하여 미니게임의 결과 전달.
	UFUNCTION()
	virtual void MinigamePass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//조건을 만족하기 위한 변수 및 해당 서브시스템에 저장된 미니게임의 결과를 None으로 변경
	UFUNCTION()
	virtual void InitCondition();

	//조건이 만족되었다면 결과를 저장해줄 클래스로 전달
	UFUNCTION()
	virtual void SendConditionToMinigameSubsystem(ESLMinigameResult Result);

	UFUNCTION()
	USLMinigameCondSubsystem* GetMinigameCondSubsystem();
protected:
	UPROPERTY(EditAnywhere)
	ESLMinigameStage AssignedMinigame = ESLMinigameStage::EMS_None;
};
