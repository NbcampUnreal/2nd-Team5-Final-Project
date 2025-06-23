#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoidMovementComponent.generated.h"

class UNiagaraSystem;
class ASwarmAgent;
class ASwarmManager;
class USoundCue;

UENUM(BlueprintType)
enum class EBoidMonsterState : uint8
{
	FS_None,
	FS_AbleToAttack 	UMETA(DisplayName = "Able To Attack"),
	FS_UnAbleToAttack	UMETA(DisplayName = "UnAble To Attack"),
	FS_Retreating		UMETA(DisplayName = "Retreating"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UBoidMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBoidMovementComponent();
	
	// 공격 후 얼마나 멀어질지 거리 설정
	UPROPERTY(EditAnywhere, Category = "Boid Combat")
	float RetreatDistance = 200.0f;
	
	// 후퇴 속도
	UPROPERTY(EditAnywhere, Category = "Boid Combat", meta = (ClampMin = "0.0"))
	float RetreatSpeed = 200.0f;

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

	// 텔포 후 콜리전 비활성화 시간
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float PostTeleportCollisionGracePeriod = 0.5f;
	
	// 텔포 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	float StuckDistanceThreshold = 800.0f;

	// 텔포 관련
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
	UFUNCTION()
	void RestorePawnCollision();
	
	void BeginAttack(float DeltaTime, const AActor* CurrentTarget, ASwarmAgent* SwarmAgent);
	
	FVector CalculateSeparationForce(const TArray<AActor*>& Neighbors);
	FVector CalculateAlignmentForce(const TArray<AActor*>& Neighbors);
	FVector CalculateCohesionForce(const TArray<AActor*>& Neighbors);
	FVector CalculateGoalSeekingForce();

	FVector SmoothedSteeringForce = FVector::ZeroVector;

	FTimerHandle RestoreCollisionTimerHandle;

	UPROPERTY()
	TObjectPtr<ASwarmAgent> OwnerCharacter = nullptr;

	UPROPERTY()
	TObjectPtr<ASwarmManager> SwarmManager = nullptr;

	// 속도를 줄이는 시작 반경
	UPROPERTY(EditAnywhere, Category = "Boids")
	float ArrivalSlowingRadius = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float MaxSteeringForce = 500.0f;

	float CurrentCoolDown = 3.0f;
	float TotalTime = 3.0f;
	FVector RetreatTargetLocation = FVector::ZeroVector;
	float OriginalMaxWalkSpeed;

	EBoidMonsterState CurrentState = EBoidMonsterState::FS_None;
};
