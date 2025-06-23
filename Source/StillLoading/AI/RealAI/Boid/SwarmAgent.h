#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "SwarmAgent.generated.h"

class ASwarmManager;
class UBlackboardData;
class UBehaviorTree;
class ATargetPoint;
class UBoidMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDied, ASwarmAgent*, Actor);

UCLASS()
class STILLLOADING_API ASwarmAgent : public ACharacter
{
	GENERATED_BODY()

public:
	ASwarmAgent();

	UFUNCTION()
	void SetLeader(bool IsLeader, UBehaviorTree* LeaderBehaviorTree = nullptr, UBlackboardData* LeaderBlackBoard = nullptr);
	UFUNCTION()
	void ApplyLeaderState(AAIController* AIController);
	UFUNCTION()
	void AgentDied();
	UFUNCTION()
	void PlayAttackAnim();
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

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Swarm")
	TObjectPtr<ASwarmManager> MySwarmManager;

	float LastAttackFinishTime = -100.f;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	void ApplyBerserkState();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoidMovementComponent> BoidMovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> CachedLeaderBehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBlackboardData> CachedLeaderBlackboard;

private:
	int32 AgentID = -1;
	bool bIsLeader = false;
	bool bShouldEnterBerserkOnPossess = false;
};
