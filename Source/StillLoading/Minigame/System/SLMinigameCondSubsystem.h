// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLMinigameCondSubsystem.generated.h"

UENUM()
enum class ESLMinigameResult
{
	Success, //성공
	Fail,    //실패
	Pass,    //미니게임 패스
	None     //아직 진행하지 않음
};

UENUM()
enum class ESLMinigameStage
{
	//각 이름은 챕터와 미니게임 제목으로
	Ch1Stage1,
	Ch1Stage2_A,
	Ch1Stage2_B,
	Ch1Stage3,
	Ch1Stage4,
	Ch1Stage5_A,
	Ch1Stage5_B,

	MG_Ch2_Stage1,
	MG_Ch2_Stage2_A,
	MG_Ch2_Stage2_B,
	MG_Ch2_Stage3,
	MG_Ch2_Stage4_A,
	MG_Ch2_Stage4_B,

	MG_Ch3_Stage1,
	MG_Ch3_Stage2,
	MG_Ch3_Stage3,
	MG_Ch3_Stage4,
	MG_Ch3_Stage5,
	
	None
};

UCLASS()
class STILLLOADING_API USLMinigameCondSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	ESLMinigameResult GetMinigameResult(ESLMinigameStage Minigame);
	void SetMinigameResult(ESLMinigameStage Minigame, ESLMinigameResult Result);
private:
	UPROPERTY()
	TMap<ESLMinigameStage, ESLMinigameResult> MinigameResult;
};
