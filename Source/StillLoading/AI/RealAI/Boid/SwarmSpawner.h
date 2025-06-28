#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "SwarmManager.h"
#include "GameFramework/Actor.h"
#include "SwarmSpawner.generated.h"

class UBoxComponent;
class UNiagaraSystem;
enum class EFollowerFormationType : uint8;
class UBlackboardData;
class AAIController;
class UBehaviorTree;
class USphereComponent;
class ATargetPoint;
class ASwarmAgent;
class ASwarmManager;

USTRUCT(BlueprintType)
struct FSwarmComposition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Swarm Composition")
	TSubclassOf<ASwarmAgent> AgentClass;

	UPROPERTY(EditAnywhere, Category = "Swarm Composition")
	TSubclassOf<AAIController> ControllerClass;

	UPROPERTY(EditAnywhere, Category = "Swarm Composition")
	int32 SpawnCount = 10;

	UPROPERTY(EditAnywhere, Category = "Swarm Composition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AvoidanceWeight = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Swarm Composition")
	bool bIsLeader = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm AI")
	FGenericTeamId TeamIDToAssign = FGenericTeamId(2);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnedMonstersCountUpdated, int32, DecreaseCount);

UCLASS()
class STILLLOADING_API ASwarmSpawner : public AActor
{
	GENERATED_BODY()

public:
	ASwarmSpawner();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable)
	void BeginSpawn();

	UFUNCTION(BlueprintCallable)
	int32 GetSpawnCount() const;

	UFUNCTION(BlueprintCallable)
	void ResetSpawendMonster();

	UFUNCTION(BlueprintCallable)
	void DestroyAllMonster();

	UPROPERTY(BlueprintAssignable, Category = "Monster Spawner | Events")
	FOnSpawnedMonstersCountUpdated OnMonstersUpdated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Spawner Setting")
	bool bBeginBurserkMode = false;

	UPROPERTY(EditAnywhere, Category = "AI | Spawner Setting")
	bool bEnableAutoSpawn = true;

	UPROPERTY(EditAnywhere, Category = "AI | Spawner Setting")
	EFollowerFormationType FormationType = EFollowerFormationType::CenteredSquare;

	UPROPERTY(EditAnywhere, Category = "AI | Spawner Setting")
	float FollowerMovementSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "AI | Spawner Setting")
	float LeaderMovementSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "AI | Spawner Setting")
	float DetectionRadius = 700.0f;

	/*
	 *<Sample>
	새 떼 / 물고기 떼 (Classic Flock/School) -> Separation: 1.5, Cohesion: 2.0, Alignment: 2.0, GoalSeeking: 0.5
	(뭉치고 방향을 맞추려는 힘이 강하지만, 서로 약간의 공간을 존중합니다.)

	좀비 무리 / 오합지졸 (Chaotic Horde) -> Separation: 0.5, Cohesion: 1.0, Alignment: 0.2, GoalSeeking: 50.0
	(오직 목표만을 보고 달려들며, 서로 부딪히거나 대열이 엉망이 되는 것은 전혀 신경 쓰지 않습니다.)

	혼비백산하는 동물들 (Panicked Swarm) -> Separation: 5.0, Cohesion: 3.0, Alignment: 0.5, GoalSeeking: 10.0
	(서로 부딪히지 않으려는(Separation) 생존 본능이 가장 강하고, 그 다음으로 함께 뭉치려는(Cohesion) 본능이 강합니다.)

	훈련된 병사 / 함대 (Disciplined Formation) -> Separation: 4.0, Cohesion: 5.0, Alignment: 5.0, GoalSeeking: 2.0
	(목표를 향해 움직이는 힘보다 대열을 유지하려는 내부적인 힘들이 훨씬 더 강해서, 하나의 단단한 개체처럼 움직입니다.)

	벌레 떼 / 혼돈의 구름 (Insect Cloud) -> Separation: 0.8, Cohesion: 2.5, Alignment: 0.1, GoalSeeking: 0.2
	(방향을 맞추려는 힘(Alignment)은 거의 없고, 흩어지지 않을 정도의 응집력(Cohesion)만 유지하여 혼란스러운 구름 형태를 만듭니다.)
	 */

	// 평상시 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Boids | Patrol Weights", meta = (ToolTip = "평상시 AI들이 서로의 개인 공간을 얼마나 존중할지 결정합니다. 값이 높을수록 서로를 강하게 밀어내어 넓은 대형을 유지합니다."))
	float SeparationWeight = 0.3f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Boids | Patrol Weights", meta = (ToolTip = "평상시 AI들이 주변 동료들과 얼마나 방향을 맞춰 움직일지 결정합니다. 값이 높을수록 질서정연한 움직임을 보입니다."))
	float AlignmentWeight = 0.4f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Boids | Patrol Weights", meta = (ToolTip = "평상시 AI들이 무리의 중심을 향해 얼마나 강하게 뭉치려 하는지 결정합니다. 군집의 '접착제' 역할을 하며, 높을수록 빽빽한 무리를 형성합니다."))
	float CohesionWeight = 0.5f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Boids | Patrol Weights", meta = (ToolTip = "평상시 AI들이 최종 목표(경유지)를 향해 얼마나 강하게 나아갈지를 결정합니다. 이 값이 다른 가중치에 비해 너무 높으면 대형을 무시하고 돌진합니다."))
	float GoalSeekingWeight = 1.0f;

	// 전투 시 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Boids | Combat Weights", meta = (ToolTip = "전투 시 AI들이 서로의 공간을 얼마나 확보할지 결정합니다. 값을 낮추면 적에게 더 빽빽하게 달려듭니다."))
	float CombatSeparationWeight = 1.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Boids | Combat Weights", meta = (ToolTip = "전투 시 AI들이 대열을 얼마나 맞춰 움직일지 결정합니다. 값을 낮추면 개별적으로, 더 혼란스럽게 움직이며 적을 압박합니다."))
	float CombatAlignmentWeight = 0.5f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Boids | Combat Weights", meta = (ToolTip = "전투 시 AI들이 뭉치려는 힘을 결정합니다. 이 값을 높이면 분대원들이 흩어지지 않고 함께 싸우려는 경향이 강해집니다."))
	float CombatCohesionWeight = 0.5f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Boids | Combat Weights", meta = (ToolTip = "전투 시 AI들이 목표(플레이어)를 향해 얼마나 저돌적으로 돌진할지 결정합니다. 이 값이 높을수록 다른 모든 것을 무시하고 목표를 향해 달려듭니다."))
	float CombatGoalSeekingWeight = 0.3f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnMonstersUpdated_Handler(int32 DecreaseCount);

	UPROPERTY(EditAnywhere, Category = "AI | Swarm Settings")
	TSubclassOf<ASwarmManager> SwarmManagerClass;

	UPROPERTY(EditAnywhere, Category = "AI | Swarm Settings")
	TObjectPtr<UBehaviorTree> LeaderBehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI | Swarm Settings")
	TObjectPtr<UBlackboardData> LeaderBlackBoard;

	UPROPERTY(EditAnywhere, Category = "AI | Swarm Settings")
	TArray<FSwarmComposition> SwarmCompositions;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> SpawnBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Patrol", meta = (MakeEditWidget = "true"))
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;

	UPROPERTY()
	int32 TotalSpawnCount = 0;

	UPROPERTY()
	TObjectPtr<ASwarmManager> SpawnedManager = nullptr;

	// Spawn Effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> SpawnEffectTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float EffectSpawnHeightOffset = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FRotator EffectSpawnRotation;

private:
	bool bLeaderSet = false;
};
