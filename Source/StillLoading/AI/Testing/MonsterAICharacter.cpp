#include "MonsterAICharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

AMonsterAICharacter::AMonsterAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMonsterAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMonsterAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void AMonsterAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonsterAICharacter::SetChasing(bool bEnable)
{
	bIsChasing = bEnable;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = bEnable ? 400.f : 200.f;
	}
}

void AMonsterAICharacter::SetPrimaryState(const FGameplayTag NewState)
{
	StateTags.Reset();
	StateTags.AddTag(NewState);
}

bool AMonsterAICharacter::IsInPrimaryState(const FGameplayTag StateToCheck) const
{
	return StateTags.HasTag(StateToCheck);
}

