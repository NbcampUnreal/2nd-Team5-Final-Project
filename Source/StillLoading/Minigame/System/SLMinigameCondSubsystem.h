// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLMinigameCondSubsystem.generated.h"

UENUM()
enum class EResult
{
	Success, //성공
	Fail,    //실패
	Pass,    //미니게임 패스
	None     //아직 진행하지 않음
};

UENUM()
enum class EMinigame
{
	MG_Ch1_Stage1,
	MG_Ch1_Stage2_A,
	MG_Ch1_Stage2_B,
	MG_Ch1_Stage3,
	MG_Ch1_Stage4,
	MG_Ch1_Stage5_A,
	MG_Ch1_Stage5_B,

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
	EResult GetMinigameResult(EMinigame minigame);
	void SetMinigameResult(EMinigame minigame, EResult result);
private:
	TMap<EMinigame, EResult> MinigameResult;
};
