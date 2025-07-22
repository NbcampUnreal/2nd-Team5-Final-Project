#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDeveloperBossPhaseBase.generated.h"

class ASLDeveloperBoss;
class ASLAIBaseCharacter;
class ASLLaunchableWall;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnPhaseCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnPhaseStateChanged, bool, bIsActive);

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

UCLASS(Abstract, BlueprintType)
class STILLLOADING_API ASLDeveloperBossPhaseBase : public AActor
{
    GENERATED_BODY()

public:
    ASLDeveloperBossPhaseBase();

    UFUNCTION(BlueprintCallable, Category = "Phase")
    virtual void StartPhase();
    
    UFUNCTION(BlueprintCallable, Category = "Phase")
    virtual void EndPhase();
    
    UFUNCTION(BlueprintCallable, Category = "Phase")
    virtual bool IsPhaseCompleted() const;
    
    UFUNCTION(BlueprintCallable, Category = "Phase")
    virtual void HandleLineDestroyed(int32 LineIndex);
    
    UFUNCTION(BlueprintCallable, Category = "Phase")
    virtual void HandleBossDeath(ASLAIBaseCharacter* DeadBoss);
    
    UFUNCTION(BlueprintCallable, Category = "Phase")
    virtual void HandleWallAttackFinished(ASLLaunchableWall* LaunchedWall);

    UFUNCTION(BlueprintCallable, Category = "Phase")
    void SetOwnerBoss(ASLDeveloperBoss* InOwnerBoss);
    
    UFUNCTION(BlueprintCallable, Category = "Phase")
    ASLDeveloperBoss* GetOwnerBoss() const;
    
    UFUNCTION(BlueprintCallable, Category = "Phase")
    bool IsPhaseActive() const;
    
    UFUNCTION(BlueprintCallable, Category = "Phase")
    int32 GetPhaseIndex() const;

    UPROPERTY(BlueprintAssignable, Category = "Phase")
    FSLOnPhaseCompleted OnPhaseCompleted;
    
    UPROPERTY(BlueprintAssignable, Category = "Phase")
    FSLOnPhaseStateChanged OnPhaseStateChanged;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void OnPhaseStarted();
    virtual void OnPhaseEnded();
    virtual void CheckPhaseCompletion();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase Settings")
    EDeveloperBossPhase PhaseType;

    UPROPERTY()
    TObjectPtr<ASLDeveloperBoss> OwnerBoss;
    
    bool bIsPhaseActive;
    int32 PhaseIndex;
};