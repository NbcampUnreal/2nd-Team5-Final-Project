// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLAIBaseCharacter.h"
#include "AI/NPC/SLNPC.h"
#include "SLNPCAIController.generated.h"


UCLASS()
class STILLLOADING_API ASLNPCAIController : public ASLBaseAIController
{
	GENERATED_BODY()

public:
	ASLNPCAIController();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void StartChasing(AActor* Target);
	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopChasing();
	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateAIState();
	UFUNCTION(BlueprintPure, Category = "AI")
	EAIState GetCurrentState() const { return CurrentState;}

	//~IGenericTeamAgentInterface inteface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//End of ~IGenericTeamAgentInterface inteface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRadius = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ChaseRadius = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CombatRadius = 550.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LoseInterestRadius = 1900.f;
	UPROPERTY(BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp;

	TArray<FVector> PatrolPoints;

	bool bPatrolPointsReady = false;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(BlueprintReadWrite, Category ="AI")
	TObjectPtr<UBlackboardComponent> BlackboardComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 190.f;

	FTimerHandle DelayHandle;

	static const FName PatrolLocationKey;

	bool bHasFixedTarget = false;

	float WaitTime = 2.f;

	bool IsPatrolState = true;

private:
	void InitializePatrolPoints();

	FVector CurrentTargetLocation;
	FVector LastKnownLocation;
	bool bIsChasing = false;
	EAIState CurrentState = EAIState::EAIS_Idle;
};
