#include "MonsterSpawner.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "AI/RealAI/FormationComponent.h"
#include "AI/RealAI/MonsterAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"

AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	RootComponent = SpawnArea;
	SpawnArea->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	SpawnArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpawnArea->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnMonstersByType();
}

void AMonsterSpawner::SpawnMonstersByType()
{
	FVector Origin = SpawnArea->Bounds.Origin;
	FVector Extent = SpawnArea->Bounds.BoxExtent;

	FVector StartLocation = Origin - FVector(Extent.X, Extent.Y, 0.f) + FVector(SpawnSpacing, SpawnSpacing, 0.f);

	int32 Row = 0;
	int32 Col = 0;

	AMonsterAICharacter* Leader = nullptr;

	for (const FMonsterSpawnInfo& Info : SpawnInfos)
	{
		if (!Info.Monster) continue;

		for (int32 i = 0; i < Info.Count; ++i)
		{
			FVector SpawnLocation = StartLocation + FVector(Col * SpawnSpacing, Row * SpawnSpacing, 0.f);
			FVector CapsuleHalf = FVector(0.f, 0.f, 88.f);
			float CapsuleRadius = 34.f;

			FVector CapsuleCenter = SpawnLocation + CapsuleHalf;

			FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, 88.f);
			bool bBlocked = GetWorld()->OverlapBlockingTestByChannel(
				CapsuleCenter,
				FQuat::Identity,
				ECC_Pawn,
				CapsuleShape
			);

			if (bDrawDebugSpawnCapsules)
			{
				FColor CapsuleColor = bBlocked ? FColor::Red : FColor::Green;
				DrawDebugCapsule(GetWorld(), CapsuleCenter, 88.f, CapsuleRadius, FQuat::Identity, CapsuleColor, false,
				                 3.0f);
			}

			if (!bBlocked)
			{
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				AMonsterAICharacter* Monster = GetWorld()->SpawnActor<AMonsterAICharacter>(
					Info.Monster,
					SpawnLocation,
					FRotator::ZeroRotator,
					Params
				);
				
				SpawnedMonsters.Add(Monster);

				if (Monster && !Leader)
				{
					Monster->SetLeader(); // 리더 지정
					Leader = Monster;
				}
			}

			Col++;
			if (Col * SpawnSpacing > Extent.X * 2)
			{
				Col = 0;
				Row++;
			}
		}
	}

	if (Leader && SpawnedMonsters.Num() > 1)
	{
		if (UFormationComponent* FormationComp = Leader->FindComponentByClass<UFormationComponent>())
		{
			FormationComp->SetAgentList(SpawnedMonsters);
		}
	}

	SpawnedMonsters.Reset();
}

FVector AMonsterSpawner::GetRandomSpawnLocation() const
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation NavLocation;

	if (NavSys && NavSys->GetRandomReachablePointInRadius(GetActorLocation(), 800.f, NavLocation))
	{
		return NavLocation.Location;
	}
	return GetActorLocation();
}
