#include "MonsterSpawner.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/RealAI/Blackboardkeys.h"
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

int32 AMonsterSpawner::GetSpawnCount() const
{
	return TotalMonsterCount;
}

void AMonsterSpawner::SpawnActorAtLocation(const TSubclassOf<AActor> ActorToSpawn, const FVector& SpawnLocation, FMonsterSpawnedInfo& OutMonsterInfo)
{
	if (!GetWorld() || !ActorToSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnActorAtLocation: World or ActorToSpawn is not valid."));
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ActorToSpawn,
		SpawnLocation,
		FRotator::ZeroRotator,
		Params
	);

	OutMonsterInfo.SpawnedMonster = SpawnedActor;
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnType == ESpawnType::ST_Leader)
	{
		SpawnMonstersByType();
	}
	else if (SpawnType == ESpawnType::ST_NoLeader)
	{
		SpawnMonstersWithoutLeader();
	}
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
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AActor* Monster = GetWorld()->SpawnActor<AActor>(
					Info.Monster,
					SpawnLocation,
					FRotator::ZeroRotator,
					Params
				);

				if (AMonsterAICharacter* AIMonster = Cast<AMonsterAICharacter>(Monster))
				{
					SpawnFloorEffect(AIMonster);
					AIMonster->BeginSpawning(SpawnLocation, RiseHeight);
					SpawnedMonsters.Add(AIMonster);

					if (!Leader)
					{
						Leader = AIMonster;
					}
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

	TotalMonsterCount = SpawnedMonsters.Num();
	LastMonsterCount = TotalMonsterCount;

	if (Leader && SpawnedMonsters.Num() > 1)
	{
		for (TObjectPtr<AActor> SpawnedMonster : SpawnedMonsters)
		{
			if (AMonsterAICharacter* AIMonster = Cast<AMonsterAICharacter>(SpawnedMonster))
			{
				if (!AIMonster->OnMonsterDied.IsAlreadyBound(this, &AMonsterSpawner::MonsterDied))
				{
					AIMonster->OnMonsterDied.AddDynamic(this, &AMonsterSpawner::MonsterDied);
				}
			}
		}
		
		FVector SpawnLocation = FVector(100.f, 100.f, 100.f);
		FRotator SpawnRotation = FRotator::ZeroRotator;
	}
}

void AMonsterSpawner::SpawnMonstersWithoutLeader()
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
				DrawDebugCapsule(GetWorld(), CapsuleCenter, 88.f, CapsuleRadius, FQuat::Identity, CapsuleColor, false,
				                 3.0f);
			}

			if (!bBlocked)
			{
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AActor* Monster = GetWorld()->SpawnActor<AActor>(
					Info.Monster,
					SpawnLocation,
					FRotator::ZeroRotator,
					Params
				);

				if (AMonsterAICharacter* AIMonster = Cast<AMonsterAICharacter>(Monster))
				{
					SpawnFloorEffect(AIMonster);
					AIMonster->BeginSpawning(SpawnLocation, RiseHeight);
					SpawnedMonsters.Add(AIMonster);

					if (AAIController* AIController = Cast<AAIController>(AIMonster->GetController()))
					{
						if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
						{
							BlackboardComp->SetValueAsVector(BlackboardKeys::HomeLocation, SpawnLocation);
							BlackboardComp->SetValueAsFloat(BlackboardKeys::AvailDistance, TargetDetectionRadius);
						}
					}
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

	TotalMonsterCount = SpawnedMonsters.Num();
	LastMonsterCount = TotalMonsterCount;

	if (SpawnedMonsters.Num() > 1)
	{
		for (TObjectPtr<AActor> SpawnedMonster : SpawnedMonsters)
		{
			if (AMonsterAICharacter* AIMonster = Cast<AMonsterAICharacter>(SpawnedMonster))
			{
				if (!AIMonster->OnMonsterDied.IsAlreadyBound(this, &AMonsterSpawner::MonsterDied))
				{
					AIMonster->OnMonsterDied.AddDynamic(this, &AMonsterSpawner::MonsterDied);
				}
			}
		}
	}
}

void AMonsterSpawner::ResetSpawnedMonster()
{
	for (AActor* SpawnedMonster : SpawnedMonsters)
	{
		if (IsValid(SpawnedMonster))
		{
			SpawnedMonster->Destroy();
		}
	}
}

void AMonsterSpawner::MonsterDied(AActor* DiedMonsterRef)
{
	if (!DiedMonsterRef)
	{
		return;
	}

	SpawnedMonsters.Remove(DiedMonsterRef);

	if (OnMonstersUpdated.IsBound())
	{
		int32 DecreaseCount = LastMonsterCount - SpawnedMonsters.Num();
		OnMonstersUpdated.Broadcast(DecreaseCount);
		LastMonsterCount = SpawnedMonsters.Num();
	}
}

void AMonsterSpawner::SpawnFloorEffect(const AMonsterAICharacter* MonsterActor)
{
	if (!MonsterActor || !SpawnEffect) return;

	USkeletalMeshComponent* MonsterMesh = MonsterActor->GetMesh();
	if (!MonsterMesh) return;

	const FName AttachSocketName = FName("root");

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		SpawnEffect,            
		MonsterMesh,            
		AttachSocketName,     
		FVector(0.f, 0.f, 20.f),           
		FRotator(0.f),         
		EAttachLocation::SnapToTarget, 
		true,             
		true
	);
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
