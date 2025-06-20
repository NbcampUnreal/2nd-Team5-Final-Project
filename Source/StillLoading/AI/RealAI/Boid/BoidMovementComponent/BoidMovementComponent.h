#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoidMovementComponent.generated.h"

class ASwarmManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UBoidMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBoidMovementComponent();

	// Boids 전역 파라미터들 (블루프린트에서 쉽게 수정 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Global Settings")
	float PerceptionRadius = 500.0f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<ASwarmManager> SwarmManager;

	FVector CalculateSeparationForce(const TArray<AActor*>& Neighbors);
	FVector CalculateAlignmentForce(const TArray<AActor*>& Neighbors);
	FVector CalculateCohesionForce(const TArray<AActor*>& Neighbors);
	FVector CalculateGoalSeekingForce();
};
