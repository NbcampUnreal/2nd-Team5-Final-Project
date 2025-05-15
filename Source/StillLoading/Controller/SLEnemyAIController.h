// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLAIBaseCharacter.h"
#include "AI/SLMonster/SLMonster.h"
#include "SLEnemyAIController.generated.h"

UCLASS()
class STILLLOADING_API ASLEnemyAIController : public ASLBaseAIController
{
	GENERATED_BODY()

public:
	ASLEnemyAIController();

	//~IGenericTeamAgentInterface inteface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//End of ~IGenericTeamAgentInterface inteface
	
	UPROPERTY(BlueprintReadWrite, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComp;

	TArray<FVector> PatrolPoints;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void StartChasing(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopChasing();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateAIState();
	UFUNCTION(BlueprintPure, Category = "AI")
	EAIState GetCurrentState() const { return CurrentState; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRadius = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ChaseRadius = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CombatRadius = 550.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LoseInterestRadius = 1900.f;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	UBlackboardComponent* BlackboardComp;

	static const FName PatrolLoctionKey;

	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 185.f;

	bool bHasFixedTarget = false;

	FTimerHandle DelayHandle;

	void Nothing();

	float WaitTime = 2.f;

private:
	FVector CurrentTargetLocation;
	void InitializePatrolPoints();

	bool bIsChasing = false;
	EAIState CurrentState = EAIState::Idle;
	FVector LastKnownLocation;
};
