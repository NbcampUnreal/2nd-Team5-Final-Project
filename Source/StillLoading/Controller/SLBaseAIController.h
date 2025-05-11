// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "SLBaseAIController.generated.h"

class ASLBaseCharacter;
struct FAIStimulus;
class UAISenseConfig_Damage;
class UAISenseConfig_Sight;

UCLASS()
class STILLLOADING_API ASLBaseAIController : public ADetourCrowdAIController
{
	GENERATED_BODY()

public:
	ASLBaseAIController();

protected:
	UFUNCTION()
	virtual void OnAIPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;

	virtual void SetAITeamId(const FGenericTeamId& NewTeamID);
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> AISenseConfig_Sight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Damage> AISenseConfig_Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeToRun;
	
	//Avoidance 관련 변수들
	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config")
	bool bEnableDetourCrowdAvoidance = true;

	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance",UIMin = "1",UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
	//Avoidance 관련 변수끝
	
	UPROPERTY(BlueprintReadWrite, Category = "AI|Reference")
	TObjectPtr<ASLBaseCharacter> PosseedAIPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior")
	bool bIsHostileToOtherAI;

	
};
