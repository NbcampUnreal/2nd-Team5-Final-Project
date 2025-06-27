#include "SLSkillComponent.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/Item/FallingSword.h"

USLSkillComponent::USLSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLSkillComponent::ActiveSpawnSkill(ASLPlayerCharacter* Character)
{
	const float SpawnHeight = 5000.0f;

	const FVector SpawnLocation = Character->GetActorLocation() + FVector(0, 0, SpawnHeight);
	const FRotator SpawnRotation = Character->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Cast<APawn>(Character);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (GetWorld())
	{
		GetWorld()->SpawnActor<AFallingSword>(SwordClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
	}
}

void USLSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnerInstance = GetWorld()->SpawnActor<AMonsterSpawner>(AMonsterSpawner::StaticClass(), GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
}

void USLSkillComponent::ActivateSkill(EActiveSkillType SkillType)
{
	switch (SkillType)
	{
	case EActiveSkillType::AST_Spawn:
		//ActiveSpawnSkill();
		break;
	}
	
}

