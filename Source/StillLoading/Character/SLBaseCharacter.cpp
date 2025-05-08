#include "SLBaseCharacter.h"

#include "EnhancedInputSubsystems.h"

ASLBaseCharacter::ASLBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASLBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ASLBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASLBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

