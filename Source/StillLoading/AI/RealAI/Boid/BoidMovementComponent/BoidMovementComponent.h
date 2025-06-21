#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoidMovementComponent.generated.h"

class ASwarmAgent;
class ASwarmManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UBoidMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBoidMovementComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float PerceptionRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float AttackCoolDown = 7.0f;

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

	UPROPERTY(EditAnywhere, Category = "Boids")
	float MaxSteeringForce = 500.0f;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<ASwarmManager> SwarmManager;

	// 속도를 줄이는 시작 반경
	UPROPERTY(EditAnywhere, Category = "Boids")
	float ArrivalSlowingRadius = 100.0f;

	FVector CalculateSeparationForce(const TArray<AActor*>& Neighbors);
	FVector CalculateAlignmentForce(const TArray<AActor*>& Neighbors);
	FVector CalculateCohesionForce(const TArray<AActor*>& Neighbors);
	FVector CalculateGoalSeekingForce();

	FVector SmoothedSteeringForce = FVector::ZeroVector;
};
