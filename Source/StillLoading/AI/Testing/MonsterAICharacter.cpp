#include "MonsterAICharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "GameFramework/CharacterMovementComponent.h"

AMonsterAICharacter::AMonsterAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
}

void AMonsterAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (SwordClass)
	{
		Sword = GetWorld()->SpawnActor<AActor>(SwordClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Sword, TEXT("hand_rSocket"));
		Sword->SetOwner(this);
	}

	if (ShieldClass)
	{
		Shield = GetWorld()->SpawnActor<AActor>(ShieldClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Shield, TEXT("hand_lSocket"));
		Shield->SetOwner(this);
	}

	SetPrimaryState(TAG_AI_Idle);
}

void AMonsterAICharacter::AttachItemToHand(AActor* ItemActor, const FName SocketName) const
{
	if (!ItemActor || !GetMesh()) return;

	ItemActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
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
	return StateTags.HasTagExact(StateToCheck);
}
