#pragma once

#include "CoreMinimal.h"
#include "SLDeveloperBossPhaseBase.h"
#include "SLDeveloperBossPhaseConfigs.h"
#include "SLDeveloperBossPhase5.generated.h"

class ASLLaunchableWall;
class ASLMouseActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSLOnPhase5MouseActorHit, ASLMouseActor*, HitMouseActor, int32, WallPartIndex);

UCLASS(BlueprintType)
class STILLLOADING_API ASLDeveloperBossPhase5 : public ASLDeveloperBossPhaseBase
{
    GENERATED_BODY()

public:
    ASLDeveloperBossPhase5();

    virtual void StartPhase() override;
    virtual void EndPhase() override;
    virtual bool IsPhaseCompleted() const override;

    UFUNCTION(BlueprintCallable, Category = "Phase5")
    void SetConfig(const FSLPhase5Config& InConfig);
    
    UFUNCTION(BlueprintCallable, Category = "Phase5")
    void LaunchNextWall();
    
    UFUNCTION(BlueprintCallable, Category = "Phase5")
    void LaunchMultiWallAttack();
    
    UFUNCTION(BlueprintCallable, Category = "Phase5")
    void SetMouseActor(ASLMouseActor* InMouseActor);
    
    UFUNCTION(BlueprintCallable, Category = "Phase5")
    void SetAvailableWalls(const TArray<ASLLaunchableWall*>& InWalls);

    UPROPERTY(BlueprintAssignable, Category = "Phase5")
    FSLOnPhase5MouseActorHit OnPhase5MouseActorHit;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void OnPhaseStarted() override;
    virtual void OnPhaseEnded() override;

    
    
    UFUNCTION()
    void OnWallCompleted(ASLLaunchableWall* CompletedWall);
    
    UFUNCTION()
    void OnWallHitMouseActor(ASLMouseActor* HitMouseActor, int32 WallPartIndex);
    
    UFUNCTION()
    void HandleMouseActorDestroyed(ASLMouseActor* DestroyedMouseActor);

    void LaunchSingleWall(ASLLaunchableWall* Wall);
    void LaunchReplacementWall();
    void CleanupInactiveWalls();
    void InitializeWallAttack();
    void ResetAllWalls();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase5 Settings")
    FSLPhase5Config Config;

private:
    UPROPERTY()
    TObjectPtr<ASLMouseActor> MouseActor;
    
    UPROPERTY()
    TArray<TObjectPtr<ASLLaunchableWall>> AvailableWalls;
    
    UPROPERTY()
    TArray<TObjectPtr<ASLLaunchableWall>> ActiveWalls;
    
    FTimerHandle WallAttackTimer;
    bool bIsCompleted;
};