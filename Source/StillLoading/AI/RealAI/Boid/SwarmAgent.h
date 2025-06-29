#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "SwarmAgent.generated.h"

class UCollisionRadarComponent;
class ASwarmManager;
class UBlackboardData;
class UBehaviorTree;
class ATargetPoint;
class UBoidMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDied, ASwarmAgent*, Actor);

UCLASS()
class STILLLOADING_API ASwarmAgent : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ASwarmAgent();

	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	void StartSpinning(float Degrees);
	
	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	void StopSpinning();

	UFUNCTION(BlueprintCallable, Category = "AI|ObjectPooling")
	void ActivateAgent(const FTransform& SpawnTransform);
	UFUNCTION(BlueprintCallable, Category = "AI|ObjectPooling")
	void DeactivateAgent();

	UFUNCTION()
	void SetLeader(bool IsLeader, UBehaviorTree* LeaderBehaviorTree = nullptr, UBlackboardData* LeaderBlackBoard = nullptr);
	UFUNCTION()
	void OnRadarDetectedActor(AActor* DetectedActor, float Distance);
	UFUNCTION()
	void ApplyLeaderState(AAIController* AIController);
	UFUNCTION()
	void AgentDied();
	void RotateToFaceTarget();
	UFUNCTION()
	void PlayAttackAnim();
	UFUNCTION()
	void PlayETCAnim();
	UFUNCTION()
	void PlayETCWaitAnim();
	UFUNCTION()
	void RequestBerserkMode();

	UFUNCTION()
	FORCEINLINE_DEBUGGABLE bool IsLeader() { return bIsLeader; }

	UFUNCTION()
	FORCEINLINE_DEBUGGABLE int GetAgentID() const { return AgentID; }

	UFUNCTION()
	FORCEINLINE_DEBUGGABLE void SetAgentID(const int NextAgentID) { AgentID = NextAgentID; }

	UFUNCTION(BlueprintPure, Category = "Swarm")
	ASwarmManager* GetMySwarmManager() const;

	UPROPERTY()
	FOnMonsterDied FOnMonsterDied;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APawn> CurrentDetectedActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Swarm")
	TObjectPtr<ASwarmManager> MySwarmManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement", meta = (ClampMin = "0.0"))
	float DegreesPerSecond = 1.0f;

	float LastAttackFinishTime = -100.f;
	bool bIsSpinning = false;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ApplyBerserkState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCollisionRadarComponent> CachedRadarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoidMovementComponent> CachedBoidMovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> CachedLeaderBehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBlackboardData> CachedLeaderBlackboard;

private:
	int32 AgentID = -1;
	bool bIsLeader = false;
	bool bShouldEnterBerserkOnPossess = false;
};
