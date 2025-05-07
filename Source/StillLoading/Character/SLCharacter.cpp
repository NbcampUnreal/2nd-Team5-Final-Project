#include "SLCharacter.h"


ASLCharacter::ASLCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASLCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASLCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

