#pragma once

#include "CoreMinimal.h"
#include "SLDeveloperBossPhaseBase.h"
#include "SLDeveloperBossPhaseConfigs.h"
#include "SLDeveloperBossPhase4.generated.h"

class ASLPhase4FallingFloor;
class ASLLaunchableWall;
class ULevelSequencePlayer;

UCLASS(BlueprintType)
class STILLLOADING_API ASLDeveloperBossPhase4 : public ASLDeveloperBossPhaseBase
{
	GENERATED_BODY()

public:
	ASLDeveloperBossPhase4();

	virtual void StartPhase() override;
	virtual void EndPhase() override;
	virtual bool IsPhaseCompleted() const override;

	UFUNCTION(BlueprintCallable, Category = "Phase4")
	void SetConfig(const FSLPhase4Config& InConfig);
    
	UFUNCTION(BlueprintCallable, Category = "Phase4")
	void StartAutoWallAttack();
    
	UFUNCTION(BlueprintCallable, Category = "Phase4")
	void StopAutoWallAttack();
	
	UFUNCTION(BlueprintCallable, Category = "Phase4")
	void SetAvailableWalls(const TArray<ASLLaunchableWall*>& InWalls);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPhaseStarted() override;
	virtual void OnPhaseEnded() override;
    
	UFUNCTION()
	void OnAutoWallAttackTimer();

	UFUNCTION()
	void OnCinematicFinished();

	void LaunchWallWithLines();
	void PlayStartCinematic();
	void StartPhaseAfterCinematic();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase4 Settings")
	FSLPhase4Config Config;

private:
	UPROPERTY()
	TArray<TObjectPtr<ASLLaunchableWall>> AvailableWalls;

	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> CurrentSequencePlayer;
    
	FTimerHandle AutoWallAttackTimer;
	FTimerHandle CinematicTimeoutTimer;
	bool bIsAutoWallAttackActive;
	bool bWaitingForCinematic;
	bool bIsCompleted;
};