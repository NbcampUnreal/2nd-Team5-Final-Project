#include "SwarmAgent.h"

#include "BoidMovementComponent/BoidMovementComponent.h"

ASwarmAgent::ASwarmAgent()
{
	PrimaryActorTick.bCanEverTick = false;

	BoidMovementComp = CreateDefaultSubobject<UBoidMovementComponent>("BoidMovement");
}

void ASwarmAgent::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASwarmAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

