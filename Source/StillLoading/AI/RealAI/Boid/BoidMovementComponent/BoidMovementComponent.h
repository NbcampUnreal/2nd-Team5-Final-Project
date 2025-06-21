#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoidMovementComponent.generated.h"

class UNiagaraSystem;
class ASwarmAgent;
class ASwarmManager;

UENUM(BlueprintType)
enum class EBoidMonsterState : uint8
{
	FS_None,
	FS_AbleToAttack 	UMETA(DisplayName = "Able To Attack"),
	FS_UnAbleToAttack	UMETA(DisplayName = "UnAble To Attack"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UBoidMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBoidMovementComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float PerceptionRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float MaxAttackCoolDown = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float MinAttackCoolDown = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float AttackRange = 150.0f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void HandleCombatState(float DeltaTime, ASwarmAgent* Agent);
	UFUNCTION()
	void HandleMovementState(float DeltaTime, ASwarmAgent* Agent);

	// 순간이동 관련
	UFUNCTION()
	void CheckAndHandleStuckTeleport(float DeltaTime);
	UFUNCTION()
	FVector GetGoalLocation() const;
	void InitializeComponent(const TObjectPtr<ASwarmManager>& InSwarmManager);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	float StuckDistanceThreshold = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	float ObstacleCheckDistance = 150.0f;

	// Target과의 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid | Combat")
	float DesiredAttackDistance = 100.0f;

	// 타겟과 멀어지려는 힘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid | Combat")
	float ForceDistanceToTarget = 50000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid | Orbit")
	float OrbitForceWeight = 0.8f; // 공전하는 힘의 세기 (클수록 빠르게 돕니다)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid | Orbit", meta = (ToolTip = "1.0 for counter-clockwise, -1.0 for clockwise"))
	float OrbitDirection = 1.0f; // 1.0 또는 -1.0으로 공전 방향을 제어 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	TObjectPtr<UNiagaraSystem> TeleportEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	TObjectPtr<USoundCue> TeleportSound = nullptr;

private:
	void BeginAttack(float DeltaTime, const AActor* CurrentTarget, ASwarmAgent* SwarmAgent);
	
	FVector CalculateSeparationForce(const TArray<AActor*>& Neighbors);
	FVector CalculateAlignmentForce(const TArray<AActor*>& Neighbors);
	FVector CalculateCohesionForce(const TArray<AActor*>& Neighbors);
	FVector CalculateGoalSeekingForce();

	FVector SmoothedSteeringForce = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<ASwarmAgent> OwnerCharacter = nullptr;

	UPROPERTY()
	TObjectPtr<ASwarmManager> SwarmManager = nullptr;

	// 속도를 줄이는 시작 반경
	UPROPERTY(EditAnywhere, Category = "Boids")
	float ArrivalSlowingRadius = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float MaxSteeringForce = 500.0f;

	float CurrentCollDown = 3.0f;
	float TotalTime = 3.0f;

	EBoidMonsterState CurrentState = EBoidMonsterState::FS_None;
};
