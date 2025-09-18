#pragma once

#include "CoreMinimal.h"
#include "SLDeveloperBossPhaseBase.h"
#include "SLDeveloperBossPhaseConfigs.h"
#include "SLDeveloperBossPhase1.generated.h"

class ULevelSequencePlayer;
class ASLAIBaseCharacter;
class ULevelSequence;
class ALevelSequenceActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnBossSpawnCompleted, ASLAIBaseCharacter*, SpawnedBoss);

UCLASS(BlueprintType)
class STILLLOADING_API ASLDeveloperBossPhase1 : public ASLDeveloperBossPhaseBase
{
    GENERATED_BODY()

public:
    ASLDeveloperBossPhase1();

    virtual void StartPhase() override;
    virtual void EndPhase() override;
    virtual bool IsPhaseCompleted() const override;
    virtual void HandleBossDeath(ASLAIBaseCharacter* DeadBoss) override;
    
    virtual void HandleLineDestroyed(int32 LineIndex) override;
    
    UFUNCTION(BlueprintCallable, Category = "Phase1")
    void SetConfig(const FSLPhase1Config& InConfig);
    
    UFUNCTION(BlueprintCallable, Category = "Phase1")
    ASLAIBaseCharacter* SpawnNextBoss();
    
    UFUNCTION(BlueprintCallable, Category = "Phase1")
    int32 GetBossesRemaining() const;
    
    UFUNCTION(BlueprintCallable, Category = "Phase1")
    void TestKillAllBosses();
    
    UPROPERTY(BlueprintAssignable, Category = "Phase1")
    FSLOnBossSpawnCompleted OnBossSpawnCompleted;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void OnPhaseStarted() override;
    virtual void OnPhaseEnded() override;

    UFUNCTION()
    void OnCinematicFinished();
    void ProcessNextBossAfterDeath();

    void PlayCinematicForLineDestroy(int32 DestroyedLineCount);
    void PlayCinematicForBossDeath(int32 DeadBossIndex);
    
    void WeakenBoss(ASLAIBaseCharacter* Boss);
    void CompleteBossRush();
    void RegisterBossEvents(ASLAIBaseCharacter* Boss);
    void UnregisterBossEvents(ASLAIBaseCharacter* Boss);
    void CleanupDeadBosses();
    ASLAIBaseCharacter* SpawnBossCharacter(TSubclassOf<ASLAIBaseCharacter> BossClass, const FTransform& SpawnTransform);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase1 Settings")
    FSLPhase1Config Config;

private:
    UPROPERTY()
    TArray<TObjectPtr<ASLAIBaseCharacter>> SpawnedBosses;

    UPROPERTY()
    TObjectPtr<ULevelSequencePlayer> CurrentSequencePlayer;
    FTimerHandle CinematicTimeoutTimer;
    
    bool bWaitingForCinematic;
    int32 PendingBossIndex;
    int32 CurrentBossIndex;
    int32 TotalBossCount;
};