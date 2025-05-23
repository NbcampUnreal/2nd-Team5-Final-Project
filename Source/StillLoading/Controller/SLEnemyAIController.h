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
	
	UFUNCTION(BlueprintPure, Category = "AI")
	EAIState GetCurrentState() const { return CurrentState; }

	void SetPeripheralVisionAngle(float NewAngle);

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRadius = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ChaseRadius = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CombatRadius = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LoseInterestRadius = 2200.f;

	TArray<FVector> PatrolPoints;
	
	bool bPatrolPointsReady = false;

	bool bHasFixedTarget = false;

protected:
	virtual void OnTargetPerceptionForgotten(AActor* Actor);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBlackboardComponent> BlackboardComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 185.f;

	float PatrolRadius = 2500.f;
	int32 PatrolNum = 7;

	static const FName PatrolLocationKey;

	FTimerHandle DelayHandle;
	
	bool IsPatrolState = true;

private:
	void InitializePatrolPoints();

	FVector CurrentTargetLocation;
	
	FVector LastKnownLocation;

	EAIState CurrentState = EAIState::EAIS_Idle;
};
