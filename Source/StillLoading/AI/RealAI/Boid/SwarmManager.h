#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwarmManager.generated.h"

class ATargetPoint;
class ASwarmAgent;

UENUM(BlueprintType)
enum class EFollowerFormationType : uint8
{
	BehindLeader UMETA(DisplayName = "Behind Leader Formation"),
	CenteredSquare UMETA(DisplayName = "Centered Square Formation")
};

UENUM(BlueprintType)
enum class ESquadState : uint8
{
	Patrolling_Move,
	Patrolling_Wait,
	Engaging
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonstersUpdatedSignature, int32, RemainingCount);

UCLASS()
class STILLLOADING_API ASwarmManager : public AActor
{
	GENERATED_BODY()

public:
	ASwarmManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SetLeader(ASwarmAgent* InLeader) { LeaderAgent = InLeader; }
	ASwarmAgent* GetLeader() const { return LeaderAgent.Get(); }

	void DestroyAllAgents();

	UPROPERTY(BlueprintAssignable, Category = "Monster Spawner | Events")
	FOnMonstersUpdatedSignature OnMonstersUpdated;

	UFUNCTION(BlueprintCallable)
	void SetNewPath(const TArray<FVector>& NewPathPoints);

	UPROPERTY(BlueprintReadOnly)
	int32 TotalSpawnCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 LastMonsterCount = 0;

	// 팔로워 인지 관련
	UFUNCTION()
	void ReportTargetSighting(AActor* SelfTarget, AActor* SightedTarget);
	UFUNCTION()
	void BroadcastNewTarget(AActor* NewTarget);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Squad")
	TObjectPtr<AActor> CurrentSquadTarget;

	float LastTimeTargetSeen;

	UPROPERTY(EditAnywhere, Category = "Squad")
	float TargetForgettingTime = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Squad")
	float MaxDetectionRange = 3000.0f;

	// 팔로워 초기 셋팅 관련
	void RegisterAgent(ASwarmAgent* Agent);
	void UnregisterAgent(ASwarmAgent* Agent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation", meta = (ToolTip = "각 슬롯에 적용될 최대 랜덤 오프셋 거리"))
	float MaxRandomOffset = 25.0f;

	// 리더 감지 범위
	UPROPERTY(EditAnywhere, Category = "AI | Spawner Setting")
	float DetectionRadius = 700.0f;

	const TArray<TObjectPtr<ASwarmAgent>>& GetAgents() const { return AllAgents; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowFollowersDebugLine = false;

	// 순찰용 가중치
	// 분리 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float PatrolSeparation = 0.f;
	// 정렬 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float PatrolAlignment = 0.f;
	// 응집 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float PatrolCohesion = 0.f;
	// 목표 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float PatrolGoalSeeking = 0.f;

	// 전투용 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Combat Weights")
	float CombatSeparation = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Combat Weights")
	float CombatCohesion = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Combat Weights")
	float CombatAlignment = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Combat Weights")
	float CombatGoalSeeking = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Arrival Settings")
	float ArrivalSlowingRadius = 200.0f;

	// 스포너로부터 전달받은 순찰 지점들을 저장할 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swarm State")
	TArray<TObjectPtr<ATargetPoint>> SwarmPatrolPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swarm State")
	TArray<FVector> CurrentPath;

	// 아래는 포메이션 관련
	FVector GetFormationSlotLocationForAgent(int32 AgentID) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation", meta = (ToolTip = "스폰될 AI들이 사용할 포메이션 타입을 선택합니다."))
	EFollowerFormationType CurrentFormationType = EFollowerFormationType::BehindLeader;

	UPROPERTY(EditAnywhere, Category = "Formation Settings", meta = (ClampMin = "1"))
	int32 NumColumns = 5; // 대형 넓이

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
	int32 NumRows = 3;

	UPROPERTY(EditAnywhere, Category = "Formation Settings", meta = (ClampMin = "0.0"))
	float RowSpacing = 200.0f; // 대형 앞뒤 깊이

	UPROPERTY(EditAnywhere, Category = "Formation Settings", meta = (ClampMin = "0.0"))
	float ColumnSpacing = 200.0f; // 병사들 사이 좌우 간격

	UPROPERTY(EditAnywhere, Category = "Formation Settings", meta = (ClampMin = "0.0"))
	float FormationOffsetBehindLeader = 200.0f;

	// 팔로워 상태 관련
	void SetSquadState(const ESquadState NewState);
	ESquadState GetCurrentSquadState() const { return CurrentSquadState; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swarm State")
	FVector SwarmGoalLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swarm State")
	bool bHasGoal = false;

	UFUNCTION()
	void AgentDead(ASwarmAgent* Agent);

protected:
	UFUNCTION()
	void TryToAppointNewLeader();

private:
	UPROPERTY()
	TArray<TObjectPtr<ASwarmAgent>> AllAgents;

	UPROPERTY()
	TObjectPtr<ASwarmAgent> LeaderAgent;

	UPROPERTY()
	int32 MaxConcurrentAttackers = 3;

	UPROPERTY()
	TMap<int32, FVector> AgentRandomOffsets;

	TArray<FVector> CurrentPathPoints;

	ESquadState CurrentSquadState = ESquadState::Patrolling_Wait;
	int32 NextAgentID = 0;
	int32 CurrentPathIndex;
};
