#include "SwarmManager.h"

#include "SwarmAgent.h"

ASwarmManager::ASwarmManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASwarmManager::BeginPlay()
{
	Super::BeginPlay();
}

void ASwarmManager::InitializeSwarm(const TSubclassOf<ASwarmAgent>& AgentClass, const int32 NumAgents, const float Radius, const TArray<ATargetPoint*>& InPatrolPoints)
{
	SwarmPatrolPoints = InPatrolPoints;
	
	if (!AgentClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < NumAgents; ++i)
	{
		FVector SpawnLocation = GetActorLocation() + FMath::VRand() * FMath::FRandRange(0, Radius);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
		World->SpawnActor<ASwarmAgent>(AgentClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
}

