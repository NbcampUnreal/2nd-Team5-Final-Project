#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDeveloperBoss.generated.h"

class ASLAIBaseCharacter;
class ASLBossCharacter;
class ASLDeveloperRoomCable;
class ASLLaunchableWall;
class ASLDeveloperRoomSpace;
class ASLMouseActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossCharacterDeath, ASLAIBaseCharacter*, DeadCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossPatternFinished, ASLAIBaseCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeveloperBossPatternFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossLineDestroyed, int32, LineIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, int32, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase1BossRushCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase2HackSlashCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase3HorrorCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhase5FinalCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnBossSpawnCompleted, ASLAIBaseCharacter*, SpawnedBoss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnWallCooldownFinished);

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

UCLASS()
class STILLLOADING_API ASLDeveloperBoss : public AActor
{
    GENERATED_BODY()

public:
    ASLDeveloperBoss();
    
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
    void LaunchWallAttack();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    EDeveloperBossPhase GetCurrentPhase() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    int32 GetDestroyedLinesCount() const;

    UFUNCTION(BlueprintCallable, Category = "Developer Boss")
    bool CanLaunchWallAttack() const;

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
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestKillAllBosses();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestDestroyNextLine();
    
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
    FSLOnPhase1BossRushCompleted OnPhase1BossRushCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase2HackSlashCompleted OnPhase2HackSlashCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase3HorrorCompleted OnPhase3HorrorCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnPhase5FinalCompleted OnPhase5FinalCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnBossSpawnCompleted OnBossSpawnCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Developer Boss")
    FSLOnWallCooldownFinished OnWallCooldownFinished;

protected:
    UFUNCTION()
    void HandleBossDeath(ASLAIBaseCharacter* DeadBoss);
    
    UFUNCTION()
    void HandlePatternFinished(ASLAIBaseCharacter* Boss);

    UFUNCTION()
    void HandleLineDestroyed(int32 LineIndex);

    UFUNCTION()
    void HandleWallAttackFinished();

    UFUNCTION()
    void HandleMouseActorDestroyed(ASLMouseActor* DestroyedMouseActor);

    UFUNCTION()
    void HandlePhase2RoomEscape(ASLDeveloperRoomSpace* Room);

    UFUNCTION()
    void OnWallCooldownFinishedInternal();

    UFUNCTION()
    void OnPhase1SpawnDelayFinished();

    UFUNCTION()
    void OnPhase3SurvivalTimeFinished();
    void SpawnPhase3MouseActor();

    void RegisterBossEvents(ASLAIBaseCharacter* Boss);
    void UnregisterBossEvents(ASLAIBaseCharacter* Boss);
    void SetupBossLines();
    void ActivateNextLine();
    void StartPhasePattern(EDeveloperBossPhase Phase);
    void ChangePhase(EDeveloperBossPhase NewPhase);

    void StartPhase1BossRush();
    void SpawnNextPhase1Boss();
    void HandlePhase1BossDeath(ASLAIBaseCharacter* DeadBoss);
    void CompletePhase1BossRush();
    void WeakenBossForPhase1(ASLAIBaseCharacter* Boss);

    void StartPhase3Horror();
    void StartPhase5Final();
    void CleanupDeadBosses();
    void ResetCurrentWall();
    void StartPhase2HackSlash();

    UFUNCTION()
    void HandlePhase3MouseActorDestroyed(ASLMouseActor* DestroyedMouseActor);
    
    void DestroyPhase3MouseActor();
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Developer Boss")
    TArray<TSubclassOf<ASLAIBaseCharacter>> AvailableBossClasses;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Developer Boss")
    TArray<TObjectPtr<ASLAIBaseCharacter>> SpawnedBosses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Lines")
    TArray<TObjectPtr<ASLDeveloperRoomCable>> BossLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Wall")
    TArray<TObjectPtr<ASLLaunchableWall>> LaunchableWalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Wall")
    float WallAttackCooldown;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Developer Boss|Wall")
    TObjectPtr<ASLLaunchableWall> CurrentWall;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    float Phase1BossHealthMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    float Phase1BossSpawnDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    FVector Phase1BossSpawnOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase2")
    TObjectPtr<ASLDeveloperRoomSpace> Phase2Room;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Developer Boss|Mouse Actor")
    TObjectPtr<ASLMouseActor> MouseActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Mouse Actor")
    TSubclassOf<ASLMouseActor> MouseActorClass;
    
private:
    static const int32 MaxBossLines = 5;

    EDeveloperBossPhase CurrentPhase;
    int32 DestroyedLinesCount;
    bool bIsFightStarted;
    bool bCanLaunchWall;

    bool bIsPhase1Active;
    int32 Phase1CurrentBossIndex;
    int32 Phase1TotalBossCount;

    bool bIsPhase2Active;

    bool bIsPhase3Active;
    bool bIsPhase5Active;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase3", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = "true"))
    float Phase3SurvivalTime;

    
    TObjectPtr<ASLMouseActor> Phase3MouseActor;
};