#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SLDeveloperBossPhaseConfigs.generated.h"

class ASLAIBaseCharacter;
class ULevelSequence;
class ASLDeveloperRoomSpace;
class ASLMouseActor;
class ASLPhase4FallingFloor;
class ASLLaunchableWall;

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLPhase1Config
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Settings")
    TArray<TSubclassOf<ASLAIBaseCharacter>> BossClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Settings")
    float BossHealthMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Settings")
    float BossSpawnDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Settings")
    FVector BossSpawnOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematics")
    TArray<TObjectPtr<ULevelSequence>> Cinematics;

    FSLPhase1Config()
    {
        BossHealthMultiplier = 0.3f;
        BossSpawnDelay = 2.0f;
        BossSpawnOffset = FVector(300.0f, 0.0f, 0.0f);
    }
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLPhase2Config
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Settings")
    TObjectPtr<ASLDeveloperRoomSpace> RoomSpace;

    FSLPhase2Config()
    {
        RoomSpace = nullptr;
    }
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLPhase3Config
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float AutoWallAttackInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float InitialWallAttackDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    bool bRandomWallSelection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Actor Settings")
    TSubclassOf<ASLMouseActor> MouseActorClass;

    FSLPhase3Config()
    {
        AutoWallAttackInterval = 4.0f;
        InitialWallAttackDelay = 2.0f;
        bRandomWallSelection = false;
        MouseActorClass = nullptr;
    }
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLPhase4Config
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Settings")
    TObjectPtr<ASLPhase4FallingFloor> FallingFloor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Settings")
    float FloorCollapseDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float AutoWallAttackInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float InitialWallAttackDelay;

    FSLPhase4Config()
    {
        FallingFloor = nullptr;
        FloorCollapseDelay = 1.0f;
        AutoWallAttackInterval = 4.0f;
        InitialWallAttackDelay = 2.0f;
    }
};

USTRUCT(BlueprintType)
struct STILLLOADING_API FSLPhase5Config
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings", meta = (ClampMin = "1", ClampMax = "10"))
    int32 MaxSimultaneousWalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float MultiWallDelayMin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float MultiWallDelayMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    bool bEnableMultiWallAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float WallAttackInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float WallAttackDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    float WallResetDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings", meta = (ClampMin = "1", ClampMax = "10"))
    int32 MaxActiveWalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Attack Settings")
    bool bLimitActiveWalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Available Walls")
    TArray<TObjectPtr<ASLLaunchableWall>> AvailableWalls;

    FSLPhase5Config()
    {
        MaxSimultaneousWalls = 3;
        MultiWallDelayMin = 0.2f;
        MultiWallDelayMax = 1.0f;
        bEnableMultiWallAttack = true;
        WallAttackInterval = 2.0f;
        WallAttackDelay = 1.0f;
        WallResetDelay = 1.5f;
        MaxActiveWalls = 2;
        bLimitActiveWalls = true;
    }
};

UCLASS(BlueprintType)
class STILLLOADING_API USLDeveloperBossPhaseConfigDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Configurations")
    FSLPhase1Config Phase1Config;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Configurations")
    FSLPhase2Config Phase2Config;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Configurations")
    FSLPhase3Config Phase3Config;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Configurations")
    FSLPhase4Config Phase4Config;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Configurations")
    FSLPhase5Config Phase5Config;
};