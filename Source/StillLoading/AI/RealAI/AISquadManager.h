#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISquadManager.generated.h"

class AAIController;
class UBlackboardComponent;
enum class EOrderType : uint8;
class UFormationComponent;

UCLASS()
class STILLLOADING_API AAISquadManager : public AActor
{
	GENERATED_BODY()

public:
	AAISquadManager();

	UFUNCTION(BlueprintCallable, Category = "AI Squad|Orders")
	void OrderPatrol();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFormationComponent> FormationComponent;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Squad|Members")
	TObjectPtr<AActor> Leader;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Squad|Members")
	TArray<TObjectPtr<AActor>> SquadFollowers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Squad|State")
	TArray<TObjectPtr<AActor>> AttackingAgents;

	bool bIsPatrolling = false;
    
	FVector CurrentPatrolDestination;

	UPROPERTY(EditAnywhere, Category = "AI Squad|Patrol")
	float PatrolDestinationReachedTolerance = 100.f;

	UPROPERTY()
	TObjectPtr<AAIController> LeaderController;
    
	UPROPERTY()
	TObjectPtr<UBlackboardComponent> SharedBlackboard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Squad|Patrol")
	float PatrolRadiusFromLeader = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Squad|Patrol")
	float TargetDetectionRadius = 1500.f;

public:
	UFUNCTION(BlueprintCallable, Category = "AI Squad")
	void InitializeSquad(const TArray<AActor*>& AllMembers);

	UFUNCTION(BlueprintCallable, Category = "AI Squad")
	void Order(EOrderType OrderType);

private:
	void Attack();
	void HoldPosition(bool bIsAll = false);
	void CleanUpInvalidAgents();
	void Patrol();

	bool bIsInBerserkMode = false;
};
