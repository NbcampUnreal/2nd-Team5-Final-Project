#include "SLCharacter.h"

#include "GamePlayTag/GamePlayTag.h"

ASLCharacter::ASLCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	
}

void ASLCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (SwordClass)
	{
		Sword = GetWorld()->SpawnActor<AActor>(SwordClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Sword, TEXT("r_weapon_socket"));
	}

	if (ShieldClass)
	{
		Shield = GetWorld()->SpawnActor<AActor>(ShieldClass, GetActorLocation(), GetActorRotation());
		AttachItemToHand(Shield, TEXT("l_weapon_socket"));
	}

	SetPrimaryState(TAG_Character_Movement_Dash);

	if (PrimaryStateTags.HasTag(TAG_Character_Movement_Run))
	{
		UE_LOG(LogTemp, Warning, TEXT("Run"));
	}

	PrintPrimaryStateTags();
}

void ASLCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASLCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASLCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	LastLandTime = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Log, TEXT("Landed at time: %f"), LastLandTime);
}

void ASLCharacter::AttachItemToHand(AActor* ItemActor, const FName SocketName) const
{
	if (!ItemActor || !GetMesh()) return;

	ItemActor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
}

// 상태 관리
void ASLCharacter::SetPrimaryState(FGameplayTag NewState)
{
	PrimaryStateTags.AddTag(NewState);
}

void ASLCharacter::AddSecondaryState(FGameplayTag NewState)
{
	SecondaryStateTags.AddTag(NewState);
}

void ASLCharacter::RemoveSecondaryState(FGameplayTag StateToRemove)
{
	SecondaryStateTags.RemoveTag(StateToRemove);
}

void ASLCharacter::SetMovementState(FGameplayTag NewMovementState)
{
	TArray<FGameplayTag> TagsToRemove;
	for (const FGameplayTag& Tag : PrimaryStateTags.GetGameplayTagArray())
	{
		if (Tag.MatchesTag(TAG_Character_Movement))
		{
			TagsToRemove.Add(Tag);
		}
	}
	for (const FGameplayTag& Tag : TagsToRemove)
	{
		PrimaryStateTags.RemoveTag(Tag);
	}

	PrimaryStateTags.AddTag(NewMovementState);
}

bool ASLCharacter::IsInPrimaryState(FGameplayTag StateToCheck) const
{
	return PrimaryStateTags.HasTag(StateToCheck);
}

bool ASLCharacter::HasSecondaryState(FGameplayTag StateToCheck) const
{
	return SecondaryStateTags.HasTag(StateToCheck);
}

bool ASLCharacter::IsInMovementState() const
{
	return PrimaryStateTags.HasTag(TAG_Character_Movement);
}

bool ASLCharacter::IsInAttackState() const
{
	return PrimaryStateTags.HasTag(TAG_Character_Attack);
}

bool ASLCharacter::IsInDefenseState() const
{
	return PrimaryStateTags.HasTag(TAG_Character_Defense);
}

bool ASLCharacter::IsInHitReactionState() const
{
	return PrimaryStateTags.HasTag(TAG_Character_HitReaction);
}


void ASLCharacter::PrintPrimaryStateTags() const
{
	for (const FGameplayTag& Tag : PrimaryStateTags.GetGameplayTagArray())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PrimaryState] %s"), *Tag.ToString());
	}
}
