#include "MonsterSpawner.h"

#include "NavigationSystem.h"
#include "AI/Testing/MonsterAICharacter.h"
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
                DrawDebugCapsule(GetWorld(), CapsuleCenter, 88.f, CapsuleRadius, FQuat::Identity, CapsuleColor, false, 3.0f);
            }

            if (!bBlocked)
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AMonsterAICharacter* Monster = GetWorld()->SpawnActor<AMonsterAICharacter>(
                    Info.Monster,
                    SpawnLocation,
                    FRotator::ZeroRotator,
                    Params
                );
            }

            Col++;
            if (Col * SpawnSpacing > Extent.X * 2)
            {
                Col = 0;
                Row++;
            }
        }
    }
}

void AMonsterSpawner::SpawnAllMonsters()
{
	for (const FMonsterSpawnInfo& Info : SpawnInfos)
	{
		if (!Info.Monster) continue;

		for (int32 i = 0; i < Info.Count; ++i)
		{
			FVector SpawnLoc = GetRandomSpawnLocation();
			FRotator SpawnRot = FRotator::ZeroRotator;

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AMonsterAICharacter* Monster = GetWorld()->SpawnActor<AMonsterAICharacter>(Info.Monster, SpawnLoc, SpawnRot, Params);
			if (Monster)
			{
				// 필요시 무기 장착, 커스텀 컴포넌트 초기화 등
			}
		}
	}
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