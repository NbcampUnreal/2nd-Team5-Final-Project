#include "SLSkillComponent.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/Item/FallingSword.h"

USLSkillComponent::USLSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnerInstance = GetWorld()->SpawnActor<AMonsterSpawner>(AMonsterSpawner::StaticClass(), GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
}

void USLSkillComponent::ActiveSpawnSkill()
{
	if (SpawnerInstance)
	{
		FMonsterSpawnedInfo OutMonsterInfo;
		SpawnerInstance->SpawnActorAtLocation(SpawnInfo.Monster, GetOwner()->GetActorLocation(), OutMonsterInfo);

		if (OutMonsterInfo.SpawnedMonster)
		{
			const FRotator OwnerRotation = GetOwner()->GetActorRotation();
			const FRotator LookDirection = FRotator(0.0f, OwnerRotation.Yaw, 0.0f);

			OutMonsterInfo.SpawnedMonster->SetActorRotation(LookDirection);
			OutMonsterInfo.SpawnedMonster->SetLifeSpan(7.0f);
		}

		if (ASLPlayerCharacter* DummyPlayerCharacter = Cast<ASLPlayerCharacter>(OutMonsterInfo.SpawnedMonster))
		{
			DummyPlayerCharacter->Shield->Destroy();
			DummyPlayerCharacter->SwordFromSky();

			const float SpawnHeight = 5000.0f;
			const FVector SpawnLocation = DummyPlayerCharacter->GetActorLocation() + FVector(0, 0, SpawnHeight);

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = DummyPlayerCharacter;
			SpawnParams.Instigator = Cast<APawn>(DummyPlayerCharacter);
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (GetWorld())
			{
				GetWorld()->SpawnActor<AFallingSword>(SwordClassToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}
		}
	}
}

void USLSkillComponent::ActiveSpawnSkillTemp()
{
	const float SpawnHeight = 5000.0f;
	const FVector SpawnLocation = GetOwner()->GetActorLocation() + FVector(0, 0, SpawnHeight);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (GetWorld())
	{
		GetWorld()->SpawnActor<AFallingSword>(SwordClassToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
}

void USLSkillComponent::ActivateSkill(EActiveSkillType SkillType)
{
	switch (SkillType)
	{
	case EActiveSkillType::AST_Spawn:
		//ActiveSpawnSkill();
		ActiveSpawnSkillTemp();
		break;
	}
	
}

