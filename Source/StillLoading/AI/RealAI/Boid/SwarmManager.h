// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwarmManager.generated.h"

class ATargetPoint;
class ASwarmAgent;

UCLASS()
class STILLLOADING_API ASwarmManager : public AActor
{
	GENERATED_BODY()

public:
	ASwarmManager();

	// 스포너로부터 호출될 초기화 함수
	void InitializeSwarm(const TSubclassOf<ASwarmAgent>& AgentClass, int32 NumAgents, float Radius, const TArray<ATargetPoint*>& InPatrolPoints);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swarm State")
	FVector SwarmGoalLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swarm State")
	bool bHasGoal = false;
	
	// 분리 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float SeparationWeight = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float AlignmentWeight = 1.0f;

	// 응집 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float CohesionWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float GoalSeekingWeight = 0.8f;

	// 스포너로부터 전달받은 순찰 지점들을 저장할 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swarm State")
	TArray<TObjectPtr<ATargetPoint>> SwarmPatrolPoints;
	
protected:
	virtual void BeginPlay() override;
};
