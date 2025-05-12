// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLMinigameCondSubsystem.generated.h"

UENUM()
enum class ESLMinigameResult : uint8
{
	EMR_None,     //아직 진행하지 않음
	EMR_Success, //성공
	EMR_Fail,    //실패
	EMR_Pass    //미니게임 패스
};

UENUM()
enum class ESLMinigameStage : uint8
{
	EMS_None,
	EMS_Ch1OpenCastleGate,
	EMS_Ch1DebugRoom,
	EMS_Ch1CarriageDriving,
	EMS_Ch1DefenseGame,
	EMS_Ch1Wayfinding,
	EMS_Ch1PathClearing,
	EMS_Ch1TreasureRun,

	EMS_Ch2VillageDefense,
	EMS_Ch2DebugRoom,
	EMS_Ch2CarriageDriving,
	EMS_Ch2WarDecision,
	EMS_Ch2MonsterClearing,
	EMS_Ch2TreasureRun,

	EMS_Ch3GateDisruption,
	EMS_Ch3CarriageAmbush,
	EMS_Ch3WarTriggerDefense,
	EMS_Ch3WarTriggerLogistics,
	EMS_Ch3RouteBlockade,
	EMS_Ch3TrapSetup
};

UCLASS()
class STILLLOADING_API USLMinigameCondSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION()
	ESLMinigameResult GetMinigameResult(ESLMinigameStage InMinigame);

	UFUNCTION()
	void SetMinigameResult(ESLMinigameStage InMinigame, ESLMinigameResult InResult);
private:

	//SaveSystem이 생기면 해당 시스템에 아래 변수를 옮기고 내부 함수들을 SaveSystem에 접근하여 Get 또는 Set하도록 변경
	UPROPERTY()
	TMap<ESLMinigameStage, ESLMinigameResult> MinigameResult;
};
