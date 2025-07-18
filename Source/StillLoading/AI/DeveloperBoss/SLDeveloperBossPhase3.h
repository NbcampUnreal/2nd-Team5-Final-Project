#pragma once

#include "CoreMinimal.h"
#include "SLDeveloperBossPhaseBase.h"
#include "SLDeveloperBossPhaseConfigs.h"
#include "SLDeveloperBossPhase3.generated.h"

class ASLMouseActor;
class ASLLaunchableWall;

UCLASS(BlueprintType)
class STILLLOADING_API ASLDeveloperBossPhase3 : public ASLDeveloperBossPhaseBase
{
    GENERATED_BODY()

public:
    ASLDeveloperBossPhase3();

    virtual void StartPhase() override;
    virtual void EndPhase() override;
    virtual bool IsPhaseCompleted() const override;
    virtual void HandleLineDestroyed(int32 LineIndex) override;

    UFUNCTION(BlueprintCallable, Category = "Phase3")
    void SetConfig(const FSLPhase3Config& InConfig);
    
    UFUNCTION(BlueprintCallable, Category = "Phase3")
    void StartAutoWallAttack();
    
    UFUNCTION(BlueprintCallable, Category = "Phase3")
    void StopAutoWallAttack();
    
    UFUNCTION(BlueprintCallable, Category = "Phase3")
    bool IsAutoWallAttackActive() const;
    
    UFUNCTION(BlueprintCallable, Category = "Phase3")
    void SetMouseActor(ASLMouseActor* InMouseActor);
    
    UFUNCTION(BlueprintCallable, Category = "Phase3")
    void SetAvailableWalls(const TArray<ASLLaunchableWall*>& InWalls);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void OnPhaseStarted() override;
    virtual void OnPhaseEnded() override;

    UFUNCTION()
    void OnAutoWallAttackTimer();
    
    UFUNCTION()
    void HandlePhase3MouseActorDestroyed(ASLMouseActor* DestroyedMouseActor);

    void SpawnPhase3MouseActor();
    void DestroyPhase3MouseActor();
    ASLLaunchableWall* GetNextWall();
    void ResetWallIndex();
    void LaunchWallWithLines(ASLLaunchableWall* Wall);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase3 Settings")
    FSLPhase3Config Config;

private:
    UPROPERTY()
    TObjectPtr<ASLMouseActor> MainMouseActor;
    
    UPROPERTY()
    TObjectPtr<ASLMouseActor> Phase3MouseActor;
    
    UPROPERTY()
    TArray<TObjectPtr<ASLLaunchableWall>> AvailableWalls;
    
    FTimerHandle AutoWallAttackTimer;
    bool bIsAutoWallAttackActive;
    int32 CurrentWallIndex;
    bool bIsCompleted;
};