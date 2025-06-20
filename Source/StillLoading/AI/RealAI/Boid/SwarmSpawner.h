#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwarmSpawner.generated.h"

class ATargetPoint;
class ASwarmAgent;
class ASwarmManager;

UCLASS()
class STILLLOADING_API ASwarmSpawner : public AActor
{
	GENERATED_BODY()

public:
	ASwarmSpawner();

	UFUNCTION(BlueprintCallable)
	void BeginSpawn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Swarm Settings")
	TSubclassOf<ASwarmManager> SwarmManagerClass;

	UPROPERTY(EditAnywhere, Category = "Swarm Settings")
	TSubclassOf<ASwarmAgent> SwarmAgentClass;

	UPROPERTY(EditAnywhere, Category = "Swarm Settings")
	int32 NumAgentsToSpawn = 50;
    
	UPROPERTY(EditAnywhere, Category = "Swarm Settings")
	float SpawnRadius = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "AI | Patrol", meta = (MakeEditWidget = "true"))
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;
};
