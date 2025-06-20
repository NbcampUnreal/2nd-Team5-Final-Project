#include "SwarmSpawner.h"

#include "SwarmManager.h"

ASwarmSpawner::ASwarmSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASwarmSpawner::BeginSpawn()
{
	UWorld* World = GetWorld();
	if (World && SwarmManagerClass)
	{
		if (ASwarmManager* SpawnedManager = World->SpawnActor<ASwarmManager>(SwarmManagerClass, GetActorLocation(), GetActorRotation()))
		{
			SpawnedManager->InitializeSwarm(SwarmAgentClass, NumAgentsToSpawn, SpawnRadius, PatrolPoints);
		}
	}
}

void ASwarmSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	BeginSpawn();
}

