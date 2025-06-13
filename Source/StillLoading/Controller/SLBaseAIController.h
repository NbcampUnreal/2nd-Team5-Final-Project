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
	
protected:
	UFUNCTION()
	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	virtual void OnTargetPerceptionForgotten(AActor* Actor);
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void SetAITeamId(const FGenericTeamId& NewTeamID);

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
	
	UPROPERTY(BlueprintReadWrite, Category = "AI|Reference")
	TObjectPtr<ASLPlayerCharacterBase> PosseedAIPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior")
	bool bIsHostileToOtherAI;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Targeting")
	float TargetEvaluationInterval;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Targeting")
	float MaxTargetSwitchDistance;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Targeting")
	float DamageThreatMultiplier;

private:
	/**
	 * @brief AI가 인식한 잠재적인 타겟들을 저장하는 맵입니다.
	 *        키는 타겟으로 인식된 액터(AActor), 값은 해당 타겟에 대한 정보(FTargetInfo)입니다.
	 *
	 * - FTargetInfo 구조체:
	 *   - Actor: 타겟 액터의 약한 참조 포인터(TWeakObjectPtr).
	 *   - Distance: AI와 타겟 간의 거리(float).
	 *   - ThreatLevel: AI가 해당 타겟에 대해 평가한 위협도(float).
	 *   - LastSeenTime: 타겟이 마지막으로 시야에 포착된 시간(float).
	 *
	 * @details
	 * 1. 타겟이 인식되거나 사라질 때마다 업데이트됩니다.
	 * 2. UpdateTargetEvaluation과 같은 함수에서 가장 적합한 타겟을 결정하기 위해 사용됩니다.
	 * 3. 사망한 타겟이나 너무 오래된 정보를 가진 타겟은 CleanupDeadTargets나 UpdateTargetEvaluation 호출 시 제거됩니다.
	 *
	 * @see AddOrUpdateTarget
	 * @see UpdateTargetEvaluation
	 * @see CleanupDeadTargets
	 */
	UPROPERTY()
	TMap<TObjectPtr<AActor>, FTargetInfo> PotentialTargets;
	UPROPERTY()
	float LastTargetEvaluationTime;
	
	UPROPERTY()
	TObjectPtr<AActor> CurrentBestTarget;
};
