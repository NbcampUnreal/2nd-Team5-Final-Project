#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDeveloperBoss.generated.h"

class ASLPhase4FallingFloor;
class ASLAIBaseCharacter;
class ASLBossCharacter;
class ASLDeveloperRoomCable;
class ASLLaunchableWall;
class ASLDeveloperRoomSpace;
class ASLMouseActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossCharacterDeath, ASLAIBaseCharacter*, DeadCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossPatternFinished, ASLAIBaseCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeveloperBossPatternFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossLineDestroyed, int32, PhaseIndex, int32, LineIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, int32, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseCompleted, int32, CompletedPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase1BossRushCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase2HackSlashCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase3HorrorCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase4PlatformerCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase5FinalCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnBossSpawnCompleted, ASLAIBaseCharacter*, SpawnedBoss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnWallCooldownFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSLOnPhase5MouseActorHit, ASLMouseActor*, HitMouseActor, int32, WallPartIndex);

UENUM(BlueprintType)
enum class EDeveloperBossPhase : uint8
{
    Phase0_Start        UMETA(DisplayName = "Phase 0 - Start"),
    Phase1_BossRush     UMETA(DisplayName = "Phase 1 - Boss Rush"),
    Phase2_HackSlash    UMETA(DisplayName = "Phase 2 - Hack & Slash"),
    Phase3_Horror       UMETA(DisplayName = "Phase 3 - Horror"),
    Phase4_Platformer   UMETA(DisplayName = "Phase 4 - Platformer"),
    Phase5_Final        UMETA(DisplayName = "Phase 5 - Final")
};

USTRUCT(BlueprintType)
struct FPendingLineActivation
{
    GENERATED_BODY()

    int32 PhaseIndex;
    TWeakObjectPtr<ASLLaunchableWall> LaunchedWall;

    FPendingLineActivation()
    {
        PhaseIndex = -1;
        LaunchedWall = nullptr;
    }
};

USTRUCT(BlueprintType)
struct FWallLineConnection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<ASLLaunchableWall> Wall;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> ConnectedLineIndices;

    FWallLineConnection()
    {
        Wall = nullptr;
        ConnectedLineIndices.Empty();
    }
};

USTRUCT(BlueprintType)
struct FPhaseLineData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TObjectPtr<ASLDeveloperRoomCable>> Lines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FWallLineConnection> WallConnections;

    FPhaseLineData()
    {
        Lines.Empty();
        WallConnections.Empty();
    }
};

UCLASS()
class STILLLOADING_API ASLDeveloperBoss : public AActor
{
    GENERATED_BODY()

public:
    ASLDeveloperBoss();
    
    // Public Functions
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    ASLAIBaseCharacter* SpawnBossCharacter(TSubclassOf<ASLAIBaseCharacter> BossClass, const FTransform& SpawnTransform);
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void DespawnAllBosses();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void InitializeBossFight();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void TriggerFirstWallDuringDialogue();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void ManualLaunchWallAttack(int32 PhaseIndex = -1, int32 WallIndex = -1);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void ManualActivateLine(int32 PhaseIndex, int32 LineIndex);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void ActivateConnectedLines(int32 PhaseIndex, ASLLaunchableWall* LaunchedWall);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    EDeveloperBossPhase GetCurrentPhase() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    int32 GetCurrentPhaseIndex() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    int32 GetPhaseDestroyedLinesCount(int32 PhaseIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    int32 GetCurrentPhaseDestroyedLinesCount() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    bool CanLaunchWallAttack(int32 PhaseIndex = -1) const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    bool IsPhaseCompleted(int32 PhaseIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    bool IsCurrentPhaseCompleted() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    bool IsPhase1Active() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    int32 GetPhase1BossesRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void SpawnMouseActor();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void DestroyMouseActor();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void ActivateMouseActor();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void DeactivateMouseActor();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    ASLMouseActor* GetMouseActor() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase3")
    void StartPhase3AutoWallAttack();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase3")
    void StopPhase3AutoWallAttack();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase3")
    bool IsPhase3AutoWallAttackActive() const;
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestKillAllBosses();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestDestroyNextLine();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestCompleteCurrentPhase();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Debug")
    void DebugPhaseData(int32 PhaseIndex);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase4")
    void StartPhase4FloorCollapse();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase4")
    void ResetPhase4Floor();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase5")
    void TriggerPhase4FloorCollapse();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase1")
    void SpawnNextPhase1Boss();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase1")
    void PlayPhase1StartCinematic();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase1")  
    void PlayPhase1BossCinematic(int32 BossIndex);
    
    // Public Variables (Delegates)
    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnBossCharacterDeath OnBossCharacterDeath;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnBossPatternFinished OnBossPatternFinished;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnDeveloperBossPatternFinished OnDeveloperBossPatternFinished;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnBossLineDestroyed OnBossLineDestroyed;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnPhaseChanged OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FOnPhaseCompleted OnPhaseCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase1BossRushCompleted OnPhase1BossRushCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase2HackSlashCompleted OnPhase2HackSlashCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase3HorrorCompleted OnPhase3HorrorCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase4PlatformerCompleted OnPhase4PlatformerCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase5FinalCompleted OnPhase5FinalCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnBossSpawnCompleted OnBossSpawnCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnWallCooldownFinished OnWallCooldownFinished;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase5MouseActorHit OnPhase5MouseActorHit;

protected:
    // Protected Functions
    UFUNCTION()
    void HandleBossDeath(ASLAIBaseCharacter* DeadBoss);
    
    UFUNCTION()
    void HandlePatternFinished(ASLAIBaseCharacter* Boss);

    UFUNCTION()
    void HandleLineDestroyed(int32 LineIndex);

    UFUNCTION()
    void HandleWallAttackFinished(ASLLaunchableWall* LaunchedWall);

    UFUNCTION()
    void HandleMouseActorDestroyed(ASLMouseActor* DestroyedMouseActor);

    UFUNCTION()
    void HandlePhase2RoomEscape(ASLDeveloperRoomSpace* Room);

    UFUNCTION()
    void OnWallCooldownFinishedInternal();

    UFUNCTION()
    void HandlePhase3MouseActorDestroyed(ASLMouseActor* DestroyedMouseActor);

    UFUNCTION()
    void ExecutePhase3WallAttack();

    UFUNCTION()
    void CheckPhase3Completion();

    UFUNCTION()
    void LaunchNextPhase5Wall();

    UFUNCTION()
    void OnPhase5MultiWallCompleted(ASLLaunchableWall* CompletedWall);

    UFUNCTION()
    void OnPhase5WallHitMouseActor(ASLMouseActor* HitMouseActor, int32 WallPartIndex);

    UFUNCTION()
    void OnPhase3AutoWallAttackTimer();

    UFUNCTION()
    void HandlePhase4FloorCollapseCompleted();
    
    
    void LaunchPhase5MultiWallAttack();
    void LaunchPhase5SingleWall(ASLLaunchableWall* Wall);
    void LaunchPhase5ReplacementWall();
    void ResetPhase5Wall(ASLLaunchableWall* WallToReset);
    void SpawnPhase3MouseActor();
    void RegisterBossEvents(ASLAIBaseCharacter* Boss);
    void UnregisterBossEvents(ASLAIBaseCharacter* Boss);
    void SetupPhaseLines();
    void StartPhasePattern(EDeveloperBossPhase Phase);
    void ChangePhase(EDeveloperBossPhase NewPhase);
    void CheckPhaseCompletion(int32 PhaseIndex);
    void LaunchSpecificWall(ASLLaunchableWall* Wall);
    void StartPhase1BossRush();
    void HandlePhase1BossDeath(ASLAIBaseCharacter* DeadBoss);
    void CompletePhase1BossRush();
    void WeakenBossForPhase1(ASLAIBaseCharacter* Boss);
    void StartPhase2HackSlash();
    void StartPhase3Horror();
    void StartPhase4Platformer();
    void StartPhase5Final();
    void CleanupDeadBosses();
    void ResetCurrentWall();
    void DestroyPhase3MouseActor();
    void InitializePhase5WallAttack();
    void ResetAllWalls();
    ASLLaunchableWall* GetNextPhase3Wall();
    void ResetPhase3WallIndex();
    void CleanupInactiveWalls();
    void LaunchPhase4WallWithLines();
    bool IsPlayerAlive() const;
    
    // Protected Variables (Settings)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Developer Boss")
    TArray<TSubclassOf<ASLAIBaseCharacter>> AvailableBossClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phases")
    TMap<int32, FPhaseLineData> PhaseLineDataMap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Wall")
    float WallAttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    float Phase1BossHealthMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    float Phase1BossSpawnDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    FVector Phase1BossSpawnOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase2")
    TObjectPtr<ASLDeveloperRoomSpace> Phase2Room;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase3")
    float Phase3WallAttackDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase3")
    int32 Phase3WallIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase3")
    float Phase3WallAttackInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase3")
    float Phase3AutoWallAttackInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase3")
    float Phase3InitialWallAttackDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase3")
    bool bPhase3RandomWallSelection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5", meta = (ClampMin = "1", ClampMax = "10"))
    int32 Phase5MaxSimultaneousWalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5")
    float Phase5MultiWallDelayMin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5")
    float Phase5MultiWallDelayMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5")
    bool bPhase5EnableMultiWallAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5")
    float Phase5WallAttackInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5")
    float Phase5WallAttackDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5")
    float Phase5WallResetDelay;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Developer Boss|Mouse Actor")
    TObjectPtr<ASLMouseActor> MouseActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Mouse Actor")
    TSubclassOf<ASLMouseActor> MouseActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5", meta = (ClampMin = "1", ClampMax = "10"))
    int32 Phase5MaxActiveWalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase5")
    bool bPhase5LimitActiveWalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase4")
    TObjectPtr<ASLPhase4FallingFloor> Phase4FallingFloor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase4")
    float Phase4FloorCollapseDelay;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase4")
    float Phase4AutoWallAttackInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase4")
    float Phase4InitialWallAttackDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    TArray<TObjectPtr<class ULevelSequence>> Phase1Cinematics;
private:
    // Private Variables (Runtime Data)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Developer Boss", meta = (AllowPrivateAccess = "true"))
    TArray<TObjectPtr<ASLAIBaseCharacter>> SpawnedBosses;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Developer Boss|Wall", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ASLLaunchableWall> CurrentWall;
    
    UPROPERTY()
    TObjectPtr<ASLLaunchableWall> CurrentPhase5Wall;
    
    EDeveloperBossPhase CurrentPhase;
    TMap<int32, int32> PhaseDestroyedLinesCount;
    bool bIsFightStarted;
    bool bCanLaunchWall;

    bool bIsPhase1Active;
    int32 Phase1CurrentBossIndex;
    int32 Phase1TotalBossCount;

    bool bIsPhase2Active;
    bool bIsPhase3Active;
    bool bIsPhase5Active;

    UPROPERTY()
    TObjectPtr<ASLMouseActor> Phase3MouseActor;
    FPendingLineActivation PendingLineActivation;

    UPROPERTY()
    TArray<TObjectPtr<ASLLaunchableWall>> Phase5AvailableWalls;
    FTimerHandle Phase3WallAttackTimer;
    FTimerHandle Phase5WallAttackTimer;
    bool bIsPhase3WallAttackScheduled;
    int32 Phase3UsedWallIndex;

    FTimerHandle Phase3AutoWallAttackTimer;
    bool bIsPhase3AutoWallAttackActive;
    int32 Phase3CurrentWallIndex;

    UPROPERTY()
    TArray<TObjectPtr<ASLLaunchableWall>> Phase5ActiveWalls;

    bool bIsPhase4Active;
    FTimerHandle Phase4AutoWallAttackTimer;
};