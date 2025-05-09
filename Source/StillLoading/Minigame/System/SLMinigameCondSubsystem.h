// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SLMinigameCondSubsystem.generated.h"

UENUM()
enum class ESLMinigameResult : uint8
{
	Success, //성공
	Fail,    //실패
	Pass,    //미니게임 패스
	None     //아직 진행하지 않음
};

UENUM()
enum class ESLMinigameStage : uint8
{
	//각 이름은 챕터와 미니게임 제목으로
	Ch1OpenCastleGate,
	Ch1DebugRoom,
	Ch1CarriageDriving,
	Ch1DefenseGame,
	Ch1Wayfinding,
	Ch1PathClearing,
	Ch1TreasureRun,

	Ch2VillageDefense,
	Ch2DebugRoom,
	Ch2CarriageDriving,
	Ch2WarDecision,
	Ch2MonsterClearing,
	Ch2TreasureRun,

	Ch3GateDisruption,
	Ch3CarriageAmbush,
	Ch3WarTriggerDefense,
	Ch3WarTriggerLogistics,
	Ch3RouteBlockade,
	Ch3TrapSetup,
	
	None
};

UCLASS()
class STILLLOADING_API USLMinigameCondSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION()
	ESLMinigameResult GetMinigameResult(ESLMinigameStage Minigame);

	UFUNCTION()
	void SetMinigameResult(ESLMinigameStage Minigame, ESLMinigameResult Result);
private:
	UPROPERTY()
	TMap<ESLMinigameStage, ESLMinigameResult> MinigameResult;
};
