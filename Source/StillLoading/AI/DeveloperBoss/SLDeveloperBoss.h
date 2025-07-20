#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDeveloperBossPhaseBase.h"
#include "SLDeveloperBossPhaseConfigs.h"
#include "SLDeveloperBoss.generated.h"

class ASLAIBaseCharacter;
class ASLDeveloperRoomCable;
class ASLLaunchableWall;
class ASLMouseActor;
class ASLDeveloperBossPhase1;
class ASLDeveloperBossPhase2;
class ASLDeveloperBossPhase3;
class ASLDeveloperBossPhase4;
class ASLDeveloperBossPhase5;
class ASLDeveloperRoomSpace;
class ASLPhase4FallingFloor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossCharacterDeath, ASLAIBaseCharacter*, DeadCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossPatternFinished, ASLAIBaseCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeveloperBossPatternFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossLineDestroyed, int32, PhaseIndex, int32, LineIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, int32, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseCompleted, int32, CompletedPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnWallCooldownFinished);

USTRUCT(BlueprintType)
struct FPendingLineActivation
{
    GENERATED_BODY()

    int32 PhaseIndex;
    TWeakObjectPtr<ASLLaunchableWall> LaunchedWall;

    FPendingLineActivation();
};

USTRUCT(BlueprintType)
struct FWallLineConnection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<ASLLaunchableWall> Wall;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> ConnectedLineIndices;

    FWallLineConnection();
};

USTRUCT(BlueprintType)
struct FPhaseLineData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TObjectPtr<ASLDeveloperRoomCable>> Lines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FWallLineConnection> WallConnections;

    FPhaseLineData();
};

UCLASS()
class STILLLOADING_API ASLDeveloperBoss : public AActor
{
    GENERATED_BODY()

public:
    ASLDeveloperBoss();
    
    // Phase Management
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void InitializeBossFight();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void StartPhase(EDeveloperBossPhase PhaseType);
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void ChangePhase(EDeveloperBossPhase NewPhase);
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    EDeveloperBossPhase GetCurrentPhase() const;
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    int32 GetCurrentPhaseIndex() const;
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    ASLDeveloperBossPhaseBase* GetCurrentPhaseActor() const;

    // Wall System
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void TriggerFirstWallDuringDialogue();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void ManualLaunchWallAttack(int32 PhaseIndex = -1, int32 WallIndex = -1);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    bool CanLaunchWallAttack(int32 PhaseIndex = -1) const;

    // Line System
    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void ManualActivateLine(int32 PhaseIndex, int32 LineIndex);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    void ActivateConnectedLines(int32 PhaseIndex, ASLLaunchableWall* LaunchedWall);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    int32 GetPhaseDestroyedLinesCount(int32 PhaseIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    int32 GetCurrentPhaseDestroyedLinesCount() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    bool IsPhaseCompleted(int32 PhaseIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    bool IsCurrentPhaseCompleted() const;

    // Mouse Actor Management
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

    // Lazy Loading - Phase Actor Management
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Lazy Loading")
    ASLDeveloperBossPhaseBase* GetPhaseActor(EDeveloperBossPhase PhaseType);
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Lazy Loading")
    void DestroyPhaseActor(EDeveloperBossPhase PhaseType);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Lazy Loading")
    void DestroyAllInactivePhases();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Lazy Loading")
    void PreloadNextPhase();

    // Test Functions
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestKillAllBosses();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestDestroyNextLine();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestCompleteCurrentPhase();

    // Debug Functions
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Debug")
    void DebugPhaseData(int32 PhaseIndex);

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Debug")
    void DebugCurrentState() const;

    // Phase Specific Functions (for backward compatibility)
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase1")
    bool IsPhase1Active() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase1")
    int32 GetPhase1BossesRemaining() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase1")
    void SpawnNextPhase1Boss();

    /*UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase1")
    void PlayPhase1StartCinematic();*/

    /*UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase1")  
    void PlayPhase1BossCinematic(int32 BossIndex);*/

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase3")
    void StartPhase3AutoWallAttack();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase3")
    void StopPhase3AutoWallAttack();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase3")
    bool IsPhase3AutoWallAttackActive() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase4")
    void StartPhase4FloorCollapse();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase4")
    void ResetPhase4Floor();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Phase4")
    void TriggerPhase4FloorCollapse();

    // Delegates
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
    FSLOnWallCooldownFinished OnWallCooldownFinished;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Event Handlers
    UFUNCTION()
    void HandlePhaseCompleted();

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
    void OnWallCooldownFinishedInternal();

    // Setup Functions
    void SetupPhaseLines();
    void LaunchSpecificWall(ASLLaunchableWall* Wall);
    void ResetCurrentWall();
    bool IsPlayerAlive() const;
    
    // Lazy Loading Helper Functions
    ASLDeveloperBossPhase1* GetOrCreatePhase1();
    ASLDeveloperBossPhase2* GetOrCreatePhase2();
    ASLDeveloperBossPhase3* GetOrCreatePhase3();
    ASLDeveloperBossPhase4* GetOrCreatePhase4();
    ASLDeveloperBossPhase5* GetOrCreatePhase5();
    
    template<typename T>
    T* CreatePhaseActor(TSubclassOf<T> PhaseClass);
    
    void SetupPhase1Actor(ASLDeveloperBossPhase1* PhaseActor, const FSLPhase1Config& Config);
    void SetupPhase2Actor(ASLDeveloperBossPhase2* PhaseActor, const FSLPhase2Config& Config);
    void SetupPhase3Actor(ASLDeveloperBossPhase3* PhaseActor, const FSLPhase3Config& Config);
    void SetupPhase4Actor(ASLDeveloperBossPhase4* PhaseActor, const FSLPhase4Config& Config);
    void SetupPhase5Actor(ASLDeveloperBossPhase5* PhaseActor, const FSLPhase5Config& Config);
    
    void SetupPhase3Walls(ASLDeveloperBossPhase3* PhaseActor);
    void SetupPhase4Walls(ASLDeveloperBossPhase4* PhaseActor);
    void SetupPhase5Walls(ASLDeveloperBossPhase5* PhaseActor);
    
    bool IsValidPhaseType(EDeveloperBossPhase PhaseType) const;
    ASLDeveloperBossPhaseBase* FindExistingPhaseActor(EDeveloperBossPhase PhaseType) const;
    bool ValidatePhaseActors() const;
    EDeveloperBossPhase GetNextPhase(EDeveloperBossPhase InCurrentPhase) const;

    // Configuration Data Asset (순수 설정값만)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TObjectPtr<USLDeveloperBossPhaseConfigDataAsset> ConfigDataAsset;

    // Phase Actors Classes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Actors")
    TSubclassOf<ASLDeveloperBossPhase1> Phase1ActorClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Actors")
    TSubclassOf<ASLDeveloperBossPhase2> Phase2ActorClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Actors")
    TSubclassOf<ASLDeveloperBossPhase3> Phase3ActorClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Actors")
    TSubclassOf<ASLDeveloperBossPhase4> Phase4ActorClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Actors")
    TSubclassOf<ASLDeveloperBossPhase5> Phase5ActorClass;

    // Lazy Loading Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lazy Loading")
    int32 MaxCachedPhases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lazy Loading")
    bool bEnablePhasePreloading;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lazy Loading")
    bool bAutoCleanupInactivePhases;

    // Phase Line System
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line System")
    TMap<int32, FPhaseLineData> PhaseLineDataMap;

    // Wall System Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall System")
    float WallAttackCooldown;

    // Mouse Actor Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Actor")
    TObjectPtr<ASLMouseActor> MouseActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Actor")
    TSubclassOf<ASLMouseActor> MouseActorClass;

    // ✅ 레벨 액터 참조 (한 곳에서만 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Actors|Phase2")
    TObjectPtr<ASLDeveloperRoomSpace> Phase2RoomSpace;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Actors|Phase4")
    TObjectPtr<ASLPhase4FallingFloor> Phase4FallingFloor;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Actors|Phase5")
    TArray<TObjectPtr<ASLLaunchableWall>> Phase5AvailableWalls;

private:
    // Phase Management (Lazy Loading)
    UPROPERTY()
    TObjectPtr<ASLDeveloperBossPhase1> Phase1Actor;
    
    UPROPERTY()
    TObjectPtr<ASLDeveloperBossPhase2> Phase2Actor;
    
    UPROPERTY()
    TObjectPtr<ASLDeveloperBossPhase3> Phase3Actor;
    
    UPROPERTY()
    TObjectPtr<ASLDeveloperBossPhase4> Phase4Actor;
    
    UPROPERTY()
    TObjectPtr<ASLDeveloperBossPhase5> Phase5Actor;

    UPROPERTY()
    TObjectPtr<ASLDeveloperBossPhaseBase> CurrentPhaseActor;

    // Lazy Loading Management
    TArray<EDeveloperBossPhase> PhaseAccessOrder;
    
    // Wall System
    UPROPERTY()
    TObjectPtr<ASLLaunchableWall> CurrentWall;

    // Core State
    EDeveloperBossPhase CurrentPhase;
    TMap<int32, int32> PhaseDestroyedLinesCount;
    bool bIsFightStarted;
    bool bCanLaunchWall;
    FPendingLineActivation PendingLineActivation;
};