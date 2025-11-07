// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "SLBaseAIController.generated.h"

class ASLPlayerCharacterBase;
struct FAIStimulus;
class UAISenseConfig_Damage;
class UAISenseConfig_Sight;

USTRUCT()
struct FTargetInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> Actor;
	
	UPROPERTY()
	float Distance;
	
	UPROPERTY()
	float ThreatLevel;
	
	UPROPERTY()
	float LastSeenTime;

	FTargetInfo();
};

UCLASS()
class STILLLOADING_API ASLBaseAIController : public ADetourCrowdAIController
{
	GENERATED_BODY()

public:
	ASLBaseAIController();
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateSightRadius(float SightRadius , float LoseSightRadius);
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetBestTarget() const;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	void OnTargetDeath(AActor* DeadActor);

	UFUNCTION(BlueprintCallable, Category = "AI|Player Detection")
	bool IsPlayerAttacking() const;
    
	UFUNCTION(BlueprintCallable, Category = "AI|Player Detection")
	bool IsPlayerPerformingSpecificAttack(FGameplayTag AttackTag) const;
    
	UFUNCTION(BlueprintCallable, Category = "AI|Player Detection") 
	ASLPlayerCharacter* GetPlayerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void SetAITeamId(const FGenericTeamId& NewTeamID);
protected:
	UFUNCTION()
	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	virtual void OnTargetPerceptionForgotten(AActor* Actor);
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateTargetEvaluation();
	UFUNCTION(BlueprintCallable, Category = "AI")
	void AddOrUpdateTarget(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "AI")
	void RemoveTarget(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "AI")
	float CalculateThreatLevel(AActor* Actor) const;
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool IsActorAlive(AActor* Actor) const;
	UFUNCTION(BlueprintCallable, Category = "AI")
	void CleanupDeadTargets();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> AISenseConfig_Sight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Damage> AISenseConfig_Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeToRun;
	
	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config")
	bool bEnableDetourCrowdAvoidance;

	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance",UIMin = "1",UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality;

	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior")
	bool bIsHostileToOtherAI;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Targeting")
	float TargetEvaluationInterval;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Targeting")
	float MaxTargetSwitchDistance;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Targeting")
	float DamageThreatMultiplier;

	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentBestTarget;
private:

	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FTargetInfo> PotentialTargets;

	UPROPERTY()
	float LastTargetEvaluationTime;
	
	
};
