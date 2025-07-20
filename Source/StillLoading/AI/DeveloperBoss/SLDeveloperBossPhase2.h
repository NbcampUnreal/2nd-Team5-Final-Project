#pragma once

#include "CoreMinimal.h"
#include "SLDeveloperBossPhaseBase.h"
#include "SLDeveloperBossPhaseConfigs.h"
#include "SLDeveloperBossPhase2.generated.h"

class ASLDeveloperRoomSpace;
class ASLMouseActor;
class ULevelSequencePlayer;

UENUM(BlueprintType)
enum class EPhase2CinematicType : uint8
{
	Start       UMETA(DisplayName = "Start"),
	Escape      UMETA(DisplayName = "Escape")
};

UCLASS(BlueprintType)
class STILLLOADING_API ASLDeveloperBossPhase2 : public ASLDeveloperBossPhaseBase
{
	GENERATED_BODY()

public:
	ASLDeveloperBossPhase2();

	virtual void StartPhase() override;
	virtual void EndPhase() override;
	virtual bool IsPhaseCompleted() const override;

	UFUNCTION(BlueprintCallable, Category = "Phase2")
	void SetConfig(const FSLPhase2Config& InConfig);
    
	UFUNCTION(BlueprintCallable, Category = "Phase2")
	void SetRoomSpace(ASLDeveloperRoomSpace* InRoomSpace);
    
	UFUNCTION(BlueprintCallable, Category = "Phase2")
	void SetMouseActor(ASLMouseActor* InMouseActor);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPhaseStarted() override;
	virtual void OnPhaseEnded() override;

	UFUNCTION()
	void HandleRoomEscape(ASLDeveloperRoomSpace* Room);

	UFUNCTION()
	void OnCinematicFinished();

	void PlayCinematic(EPhase2CinematicType CinematicType);
	void StartPhaseAfterCinematic();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase2 Settings")
	FSLPhase2Config Config;

private:
	UPROPERTY()
	TObjectPtr<ASLDeveloperRoomSpace> RoomSpace;
    
	UPROPERTY()
	TObjectPtr<ASLMouseActor> MouseActor;
    
	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> CurrentSequencePlayer;
    
	bool bIsCompleted;
	bool bWaitingForCinematic;
	EPhase2CinematicType CurrentCinematicType;
	FTimerHandle CinematicTimeoutTimer;
};