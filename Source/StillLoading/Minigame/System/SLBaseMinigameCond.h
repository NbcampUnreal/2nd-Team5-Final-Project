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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//조건을 만족하기 위한 변수 및 클리어 여부 초기화
	UFUNCTION()
	virtual void InitCondition();


	//조건이 만족되었다면 결과를 저장해줄 클래스로 전달
	UFUNCTION()
	virtual void SendCondition(ESLMinigameResult result);

protected:
	UPROPERTY(VisibleAnywhere)
	bool bIsClear = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsFail = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsPass = false;

	UPROPERTY(EditAnywhere)
	ESLMinigameStage AssignedMinigame = ESLMinigameStage::None;
};
