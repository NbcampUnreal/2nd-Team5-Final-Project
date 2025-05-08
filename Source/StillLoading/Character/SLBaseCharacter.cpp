#include "SLBaseCharacter.h"

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

