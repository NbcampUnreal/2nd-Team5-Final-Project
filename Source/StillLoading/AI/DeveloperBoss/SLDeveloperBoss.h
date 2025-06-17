#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDeveloperBoss.generated.h"

class ASLAIBaseCharacter;
class ASLBossCharacter;
class ASLDeveloperRoomCable;
class ASLLaunchableWall;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossCharacterDeath, ASLAIBaseCharacter*, DeadCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossPatternFinished, ASLAIBaseCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeveloperBossPatternFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossLineDestroyed, int32, LineIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, int32, NewPhase);

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

    //테스트용 함수
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestSpawnRandomBoss();
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestSpawnAllBosses();
    
    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestKillAllBosses();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestDestroyNextLine();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestResetAllLines();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestTriggerDialogueWall();

    UFUNCTION(BlueprintCallable, Category = "Developer Boss|Test")
    void TestStartPhase1();

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

protected:
    UFUNCTION()
    void HandleBossDeath(ASLAIBaseCharacter* DeadBoss);
    
    UFUNCTION()
    void HandlePatternFinished(ASLAIBaseCharacter* Boss);

    UFUNCTION()
    void HandleLineDestroyed(int32 LineIndex);

    UFUNCTION()
    void HandleWallAttackFinished();

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Developer Boss")
    TArray<TSubclassOf<ASLAIBaseCharacter>> AvailableBossClasses;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Developer Boss")
    TArray<ASLAIBaseCharacter*> SpawnedBosses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Lines")
    TArray<ASLDeveloperRoomCable*> BossLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Wall")
    TArray<ASLLaunchableWall*> LaunchableWalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Wall")
    float WallAttackCooldown;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Developer Boss|Wall")
    ASLLaunchableWall* CurrentWall;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    float Phase1BossHealthMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    float Phase1BossSpawnDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer Boss|Phase1")
    FVector Phase1BossSpawnOffset;

private:
    void ResetBossLines();
    void ResetCurrentWall();

    static const int32 MaxBossLines = 5;

    EDeveloperBossPhase CurrentPhase;
    int32 DestroyedLinesCount;
    bool bIsFightStarted;
    bool bCanLaunchWall;
    FTimerHandle WallCooldownTimer;

    bool bIsPhase1Active;
    int32 Phase1CurrentBossIndex;
    int32 Phase1TotalBossCount;
    FTimerHandle Phase1SpawnTimer;
};