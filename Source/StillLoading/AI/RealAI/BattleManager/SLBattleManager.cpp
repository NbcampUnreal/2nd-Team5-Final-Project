#include "SLBattleManager.h"

#include "AI/RealAI/SLMonsterAICharacterBase.h"
#include "AI/RealAI/Component/SLAICombatComponent.h"
#include "AI/RealAI/Component/SLAILODComponent.h"
#include "AI/RealAI/Component/SLAIStateComponent.h"
#include "AI/RealAI/Component/SLWaveSpawnerComponent.h"
#include "AI/RealAI/Spawner/SLSwarmSpawner.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ASLBattleManager::ASLBattleManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CurrentGlobalWaveNumber = 0;
}

void ASLBattleManager::StartWave_Implementation(const TArray<ASLSwarmSpawner*>& SpawnersToActivate, int32 WaveIndex)
{
	if (SpawnersToActivate.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleManager: StartWave에 활성화할 스포너가 전달되지 않았습니다."));
		return;
	}

	bIsBattleActive = true;
	SpawnerWaveCompletionStatus.Empty();

	UE_LOG(LogTemp, Log, TEXT("BattleManager: %d개의 스포너로 웨이브 %d를 시작합니다."), SpawnersToActivate.Num(), WaveIndex);

	for (ASLSwarmSpawner* Spawner : SpawnersToActivate)
	{
		if (IsValid(Spawner))
		{
			Spawner->StartWave(WaveIndex);
			SpawnerWaveCompletionStatus.Add(Spawner, false);
		}
	}
}

void ASLBattleManager::StartInfiniteSpawnMode_Implementation(const TArray<ASLSwarmSpawner*>& SpawnersToActivate)
{
	if (SpawnersToActivate.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleManager: StartInfiniteSpawnMode에 활성화할 스포너가 전달되지 않았습니다."));
		return;
	}

	bIsBattleActive = true;
	TotalSpawnedUnitCount = 0;
	SpawnerWaveCompletionStatus.Empty();

	UE_LOG(LogTemp, Log, TEXT("BattleManager: %d개의 스포너에 대해 무한 스폰 모드를 시작합니다."), SpawnersToActivate.Num());

	for (ASLSwarmSpawner* Spawner : SpawnersToActivate)
	{
		if(IsValid(Spawner) && IsValid(Spawner->WaveSpawnerComponent))
		{
			Spawner->WaveSpawnerComponent->bEnableInfiniteRespawn = true;
			Spawner->StartWave(0);
		}
	}
}

void ASLBattleManager::EndBattle_Implementation(bool bPlayerWon)
{
	if (!bIsBattleActive) return;
	bIsBattleActive = false;

	UE_LOG(LogTemp, Log, TEXT("SLBattleManager: 전투 종료! 승리 여부: %s"), bPlayerWon ? TEXT("승리") : TEXT("패배"));

	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->StopWaveSpawning();
			Spawner->CleanupPool();
		}
	}

	UnitActors.Empty();
	UnitLocations.Empty();
	UnitTeamIDs.Empty();
	UnitSourceSpawners.Empty();
	UnitLODComponents.Empty();
	UnitEngagedTargetIndices.Empty();
    
	UnitIndexMap.Empty();
	TeamUnitIndices.Empty();
	TargetEngagementCounts.Empty();
	EngagedUnitsPerTarget.Empty();
	UnitTargetLocations.Empty();
}

int32 ASLBattleManager::GetTotalSpawnedUnitCount_Implementation() const
{
	return TotalSpawnedUnitCount;
}

void ASLBattleManager::ResetSpawnedUnitCount_Implementation()
{
	TotalSpawnedUnitCount = 0;
}

bool ASLBattleManager::IsBattleInProgress_Implementation() const
{
	return bIsBattleActive;
}

void ASLBattleManager::StopBattle_Implementation()
{
	if (!bIsBattleActive) return;

	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->StopWaveSpawning();
			Spawner->ReturnAllActiveUnitsToPool();
		}
	}
}

void ASLBattleManager::BeginPlay()
{
	Super::BeginPlay();

	//TODO::추후에 가져오는 방식 변경 필요
	if (bIsPlayerOnly)
	{
		PrimaryTarget = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	UnitActors.Empty();
	UnitLocations.Empty();
	UnitTeamIDs.Empty();
	UnitSourceSpawners.Empty();
	UnitLODComponents.Empty();
	UnitEngagedTargetIndices.Empty();
    
	UnitIndexMap.Empty();
	TeamUnitIndices.Empty();
	TargetEngagementCounts.Empty();
	EngagedUnitsPerTarget.Empty();
	UnitTargetLocations.Empty();

	TeamUnitIndices.Reserve(1000); // 미리 공간 확보 O(1) 버킷 할당

	PrimaryTarget = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (IsValid(PrimaryTarget.Get()))
	{
		RegisterUnit(PrimaryTarget.Get(), true, nullptr);
		UE_LOG(LogTemp, Log, TEXT("BattleManager: BeginPlay에서 플레이어 등록 완료."));
	}

	BindToSpawnerEvents();
	InitializeAISupportingMode();
}

void ASLBattleManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateAllUnitLocations();
	
	if (bUseLODSystem)
	{
		UpdateAILODs();
	}

	if (bIsPlayerOnly && bUseLODSystem)
	{
		UpdateEncounterPositions();
	}
}

void ASLBattleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndBattle_Implementation(true);

	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->OnUnitSpawned.RemoveDynamic(this, &ASLBattleManager::OnSpawnerUnitSpawnedHandler);
			Spawner->OnUnitReturnedToPool.RemoveDynamic(this, &ASLBattleManager::OnSpawnerUnitReturnedToPoolHandler);
			Spawner->OnUnitActuallyDestroyed.RemoveDynamic(
				this, &ASLBattleManager::OnSpawnerUnitActuallyDestroyedHandler);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ASLBattleManager::UpdateAllUnitLocations()
{
	const int32 NumUnits = UnitActors.Num();
	for (int32 i = 0; i < NumUnits; ++i)
	{
		if (IsValid(UnitActors[i]))
		{
			UnitLocations[i] = UnitActors[i]->GetActorLocation();
		}
	}
}

void ASLBattleManager::RegisterPlayerUnit(const AController* PlayerController)
{
	if (PlayerController && PlayerController->GetPawn())
	{
		RegisterUnit(PlayerController->GetPawn(), true, nullptr);
	}
}

void ASLBattleManager::RequestNextPatrolPointForUnit(AActor* Unit)
{
	if (!IsValid(Unit)) return;

	const int32* UnitIndexPtr = UnitIndexMap.Find(Unit);
	if (!UnitIndexPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleManager: 미등록 유닛 '%s'의 순찰 지점 요청을 처리할 수 없습니다."), *Unit->GetName());
		return;
	}

	const int32 UnitIndex = *UnitIndexPtr;
	USLAIStateComponent* StateComp = Unit->FindComponentByClass<USLAIStateComponent>();
	ASLSwarmSpawner* SourceSpawner = UnitSourceSpawners[UnitIndex];

	if (IsValid(StateComp) && IsValid(SourceSpawner))
	{
		int32 TargetIndex = StateComp->GetCurrentTargetPoint();
		const FVector NextLocation = SourceSpawner->GetNextTargetPointLocation(Unit, TargetIndex);

		if (!NextLocation.IsNearlyZero())
		{
			StateComp->SetMovementTarget(NextLocation, true);
			UE_LOG(LogTemp, Log, TEXT("BattleManager: 유닛 '%s'에게 다음 순찰 지점(%d)으로 이동 명령."), *Unit->GetName(), TargetIndex);
		}
	}
}

int32 ASLBattleManager::FindNearestEnemy(int32 MyIndex, float InRange) const
{
	if (!UnitActors.IsValidIndex(MyIndex)) return INDEX_NONE;

	const FVector MyLocation = UnitLocations[MyIndex];
	const FGenericTeamId MyTeamID = UnitTeamIDs[MyIndex];
	const float RangeSq = FMath::Square(InRange);

	int32 NearestEnemyIndex = INDEX_NONE;
	float MinDistSq = FLT_MAX;

	for (int32 i = 0; i < UnitActors.Num(); ++i)
	{
		if (i == MyIndex) continue;

		const bool bIsTargetPlayer = (i == PlayerUnitIndex);
		
		if (AreEnemies(MyTeamID, UnitTeamIDs[i], bIsTargetPlayer))
		{
			const float DistSq = FVector::DistSquared(MyLocation, UnitLocations[i]);
			if (DistSq < MinDistSq && DistSq <= RangeSq)
			{
				MinDistSq = DistSq;
				NearestEnemyIndex = i;
			}
		}
	}
	return NearestEnemyIndex;
}

// Spawner Events
void ASLBattleManager::BindToSpawnerEvents()
{
	if (ManagedSpawners.IsEmpty())
	{
		TArray<AActor*> TempSpawners;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASLSwarmSpawner::StaticClass(), TempSpawners);

		for (AActor* Spawner : TempSpawners)
		{
			if (ASLSwarmSpawner* SpawnerCast = Cast<ASLSwarmSpawner>(Spawner))
			{
				ManagedSpawners.Add(SpawnerCast);
			}
		}
	}
	
	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->SetCachedBattleManager(this);
			Spawner->OnUnitSpawned.AddDynamic(this, &ASLBattleManager::OnSpawnerUnitSpawnedHandler);
			Spawner->OnUnitReturnedToPool.AddDynamic(this, &ASLBattleManager::OnSpawnerUnitReturnedToPoolHandler);
			Spawner->OnUnitActuallyDestroyed.AddDynamic(this, &ASLBattleManager::OnSpawnerUnitActuallyDestroyedHandler);
			UE_LOG(LogTemp, Log, TEXT("ASLBattleManager: 스포너 %s의 이벤트에 바인딩했습니다."), *Spawner->GetName());
		}
	}
}

void ASLBattleManager::InitializeAISupportingMode()
{
	if (!bIsPlayerOnly)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SupportReassignmentTimerHandle,
			this,
			&ASLBattleManager::ProcessSupportingAIReassignment,
			7.0f,
			true
		);
	}
}

void ASLBattleManager::OnSpawnerUnitSpawnedHandler(AActor* SpawnedUnit, ASLSwarmSpawner* SourceSpawner)
{
	if (IsValid(SpawnedUnit))
	{
		TotalSpawnedUnitCount++;
		RegisterUnit(SpawnedUnit, false, SourceSpawner);
	}

	ACharacter* SpawnedCharacter = Cast<ACharacter>(SpawnedUnit);
	if (IsValid(SpawnedCharacter))
	{
		if (SourceSpawner->PatrolPoints.Num() == 0) return;
		RequestNextPatrolPointForUnit(SpawnedCharacter);
	}
}

void ASLBattleManager::OnSpawnerUnitReturnedToPoolHandler(AActor* ReturnedUnit, ASLSwarmSpawner* SourceSpawner)
{
	if (IsValid(ReturnedUnit))
	{
		UnregisterUnit(ReturnedUnit);

		if (OnUnitUnregistered.IsBound())
		{
			OnUnitUnregistered.Broadcast(ReturnedUnit);
		}
	}
}

void ASLBattleManager::OnSpawnerUnitActuallyDestroyedHandler(AActor* DestroyedActor)
{
	if (IsValid(DestroyedActor))
	{
		UnregisterUnit(DestroyedActor);
	}
}

// Unit
void ASLBattleManager::RegisterUnit(AActor* Actor, bool bIsPlayer, ASLSwarmSpawner* SourceSpawner)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogTemp, Error, TEXT("RegisterUnit 실패: 유효하지 않은 액터가 전달되었습니다."));
		return;
	}

	if (UnitIndexMap.Contains(Actor))
	{
		UE_LOG(LogTemp, Warning, TEXT("RegisterUnit 경고: 유닛 %s는 이미 등록되어 있습니다. 등록을 건너뜁니다."), *Actor->GetName());
		return;
	}
    
	FGenericTeamId TeamId = FGenericTeamId::NoTeam;
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		if (IGenericTeamAgentInterface* ControllerTeamAgent = Cast<IGenericTeamAgentInterface>(Pawn->GetController()))
		{
			TeamId = ControllerTeamAgent->GetGenericTeamId();
		}
	}
	else if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Actor))
	{
		TeamId = TeamAgent->GetGenericTeamId();
	}

	const int32 NewIndex = UnitActors.Num();
    
	if (bIsPlayer)
	{
		PlayerUnitIndex = NewIndex;
		TeamId = 0;
	}
    
	UnitActors.Add(Actor);
	UnitLocations.Add(Actor->GetActorLocation());
	UnitTeamIDs.Add(TeamId);
	UnitSourceSpawners.Add(SourceSpawner);
	UnitLODComponents.Add(Actor->FindComponentByClass<USLAILODComponent>());
	UnitCombatComponents.Add(Actor->FindComponentByClass<USLAICombatComponent>());
	UnitTargetLocations.Add(FVector::ZeroVector);
	UnitEngagedTargetIndices.Add(INDEX_NONE);

	UnitIndexMap.Add(Actor, NewIndex);
	TeamUnitIndices.FindOrAdd(TeamId).Indices.Add(NewIndex);

	UE_LOG(LogTemp, Log, TEXT("배틀매니저: 유닛 등록 성공: %s (팀: %d)"), *Actor->GetName(), TeamId.GetId());
}

void ASLBattleManager::UnregisterUnit(AActor* Actor)
{
    if (!IsValid(Actor)) return;

    if (!UnitIndexMap.Contains(Actor))
    {
        UE_LOG(LogTemp, Warning, TEXT("UnregisterUnit 경고: %s는 등록되지 않은 유닛입니다."), *Actor->GetName());
        return;
    }

    OnUnitDestroyed(Actor);

    const int32 IndexToRemove = UnitIndexMap.FindAndRemoveChecked(Actor);
    const int32 LastIndex = UnitActors.Num() - 1;
    
    ASLSwarmSpawner* SourceSpawner = UnitSourceSpawners[IndexToRemove];

    const FGenericTeamId TeamIdOfRemovedUnit = UnitTeamIDs[IndexToRemove];
    if (FTeamIndicesArrayWrapper* Wrapper = TeamUnitIndices.Find(TeamIdOfRemovedUnit))
    {
       Wrapper->Indices.Remove(IndexToRemove);
    }
    
    if (IndexToRemove < LastIndex)
    {
       const FGenericTeamId TeamIdOfSwappedUnit = UnitTeamIDs[LastIndex];
       if (FTeamIndicesArrayWrapper* Wrapper = TeamUnitIndices.Find(TeamIdOfSwappedUnit))
       {
          Wrapper->Indices.Remove(LastIndex);
          Wrapper->Indices.Add(IndexToRemove);
       }
    }

    UnitActors.RemoveAtSwap(IndexToRemove);
    UnitLocations.RemoveAtSwap(IndexToRemove);
    UnitTeamIDs.RemoveAtSwap(IndexToRemove);
    UnitSourceSpawners.RemoveAtSwap(IndexToRemove);
    UnitLODComponents.RemoveAtSwap(IndexToRemove);
    UnitCombatComponents.RemoveAtSwap(IndexToRemove);
    UnitTargetLocations.RemoveAtSwap(IndexToRemove);
    UnitEngagedTargetIndices.RemoveAtSwap(IndexToRemove);

    if (IndexToRemove < LastIndex)
    {
       AActor* SwappedActor = UnitActors[IndexToRemove];
       UnitIndexMap.Add(SwappedActor, IndexToRemove);
    }

    if (IsValid(SourceSpawner))
    {
       if (ACharacter* CharacterToReady = Cast<ACharacter>(Actor))
       {
          SourceSpawner->MarkUnitAsReady(CharacterToReady);
       }
    }
}

void ASLBattleManager::UpdateAILODs()
{
    const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn || UnitActors.Num() == 0) return;

    const FVector PlayerLocation = PlayerPawn->GetActorLocation();
    const int32 NumUnits = UnitActors.Num();

    TArray<EAILODLevel> FinalLODs;
    FinalLODs.SetNum(NumUnits);
    TArray<int32> UnitIndicesByLOD[5];

    for (int32 i = 0; i < NumUnits; ++i)
    {
    	if (!IsValid(UnitActors[i])) continue;
    	const float DistanceSquared = FVector::DistSquared(UnitActors[i]->GetActorLocation(), PlayerLocation);
        
        EAILODLevel IdealLevel;
        if (DistanceSquared <= FMath::Square(LODDistances.MaxDetailDistance)) IdealLevel = EAILODLevel::Max;
        else if (DistanceSquared <= FMath::Square(LODDistances.HighDetailDistance)) IdealLevel = EAILODLevel::High;
        else if (DistanceSquared <= FMath::Square(LODDistances.MediumDetailDistance)) IdealLevel = EAILODLevel::Medium;
        else if (DistanceSquared <= FMath::Square(LODDistances.LowDetailDistance)) IdealLevel = EAILODLevel::Low;
        else IdealLevel = EAILODLevel::Culled;

        UnitIndicesByLOD[static_cast<int>(IdealLevel)].Add(i);
        FinalLODs[i] = IdealLevel;
    }

    const int32 HighCostUnitCount = UnitIndicesByLOD[0].Num() + UnitIndicesByLOD[1].Num();
    const int32 HighCostBudget = LODBudget.MaxLODCount + LODBudget.HighLODCount;

    if (HighCostUnitCount > HighCostBudget)
    {
        TArray<int32> HighCostIndices;
        HighCostIndices.Append(UnitIndicesByLOD[0]);
        HighCostIndices.Append(UnitIndicesByLOD[1]);

        HighCostIndices.Sort([&](const int32& A, const int32& B)
        {
        	if (!UnitActors.IsValidIndex(A) || !UnitActors.IsValidIndex(B) || !IsValid(UnitActors[A]) || !IsValid(UnitActors[B]))
        	{
				return false;
			}
			return FVector::DistSquared(UnitActors[A]->GetActorLocation(), PlayerLocation) < FVector::DistSquared(UnitActors[B]->GetActorLocation(), PlayerLocation);
        });
    	
        for (int32 i = HighCostBudget; i < HighCostIndices.Num(); ++i)
        {
            const int32 UnitIndex = HighCostIndices[i];
            FinalLODs[UnitIndex] = EAILODLevel::Medium;
        }

        for (int32 i = 0; i < HighCostBudget && i < HighCostIndices.Num(); ++i)
        {
            const int32 UnitIndex = HighCostIndices[i];
            if (i < LODBudget.MaxLODCount)
            {
                FinalLODs[UnitIndex] = EAILODLevel::Max;
            }
            else
            {
                FinalLODs[UnitIndex] = EAILODLevel::High;
            }
        }
    }
	
    for (int32 i = 0; i < NumUnits; ++i)
    {
        if (UnitLODComponents.IsValidIndex(i) && IsValid(UnitLODComponents[i]))
        {
            UnitLODComponents[i]->SetLODLevel(FinalLODs[i]);

        	// Debug
        	/*
        	FString UnitName = UnitActors[i]->GetName();
        	FString LODString = USLAILODComponent::LODLevelToString(FinalLODs[i]);
        	FColor DebugColor = FColor::White;

        	switch (FinalLODs[i])
        	{
        	case EAILODLevel::Max:    DebugColor = FColor::Red; break;
        	case EAILODLevel::High:   DebugColor = FColor::Yellow; break;
        	case EAILODLevel::Medium: DebugColor = FColor::Green; break;
        	case EAILODLevel::Low:    DebugColor = FColor::Cyan; break;
        	case EAILODLevel::Culled: DebugColor = FColor::Black; break;
        	}

        	DrawDebugString(
				GetWorld(),
				UnitActors[i]->GetActorLocation() + FVector(0, 0, 100.f),
				LODString,
				nullptr,
				DebugColor,
				0.0f,
				true
			);
			*/
        	// Debug
        }
    }
}

void ASLBattleManager::UpdateEncounterPositions()
{
    if (!PrimaryTarget.IsValid() || UnitActors.Num() == 0)
    {
        return;
    }
    const FVector PlayerLocation = PrimaryTarget->GetActorLocation();

    // ==================================================================
    // 1. 슬롯 준비 및 모든 점유 슬롯 정리
    // ==================================================================
    const TArray<FVector> MaxLOD_Slots = CalculateCirclePositions(PlayerLocation, PressurerCircleRadius, LODBudget.MaxLODCount);
    const TArray<FVector> HighLOD_Slots = CalculateCirclePositions(PlayerLocation, PressurerCircleRadius, LODBudget.HighLODCount);
    const TArray<FVector> MediumLOD_Slots = CalculateCirclePositions(PlayerLocation, MediumCircleRadius, LODBudget.MediumLODCount);

    OccupiedAttackSlots.Empty();
    OccupiedHighLODSlots.Empty();
    OccupiedMediumLODSlots.Empty();

    // 모든 유닛의 목표 위치를 우선 초기화
    for (int32 i = 0; i < UnitTargetLocations.Num(); ++i)
    {
        UnitTargetLocations[i] = FVector::ZeroVector;
    }
    
    // ==================================================================
    // 2. 모든 유닛을 순회하며 각 LOD 레벨에 맞는 슬롯 할당
    // ==================================================================
    TArray<int32> MaxLOD_Indices, HighLOD_Indices, MediumLOD_Indices;
    
    // 거리 순 정렬을 위해 모든 유닛 인덱스를 미리 수집
    TArray<int32> AllUnitIndices;
    for(int32 i = 0; i < UnitActors.Num(); ++i) AllUnitIndices.Add(i);

    // 플레이어와 가까운 순으로 유닛 정렬
    AllUnitIndices.Sort([this, &PlayerLocation](const int32& A, const int32& B) {
        return FVector::DistSquared(UnitLocations[A], PlayerLocation) < FVector::DistSquared(UnitLocations[B], PlayerLocation);
    });

    // 정렬된 유닛 순서대로 슬롯 할당
    for (const int32 UnitIndex : AllUnitIndices)
    {
        if (!UnitLODComponents.IsValidIndex(UnitIndex) || !IsValid(UnitLODComponents[UnitIndex])) continue;
        
        EAILODLevel LODLevel = UnitLODComponents[UnitIndex]->GetCurrentLODLevel();

        if (LODLevel == EAILODLevel::Max)
        {
            if (OccupiedAttackSlots.Num() < MaxLOD_Slots.Num())
            {
                const int32 SlotIndex = OccupiedAttackSlots.Num();
                OccupiedAttackSlots.Add(SlotIndex, UnitIndex);
                FVector Jitter = FVector(FMath::RandRange(-75.f, 75.f), FMath::RandRange(-75.f, 75.f), 0.f);
                UnitTargetLocations[UnitIndex] = MaxLOD_Slots[SlotIndex] + Jitter;
            }
        }
        else if (LODLevel == EAILODLevel::High)
        {
            if (OccupiedHighLODSlots.Num() < HighLOD_Slots.Num())
            {
                const int32 SlotIndex = OccupiedHighLODSlots.Num();
                OccupiedHighLODSlots.Add(SlotIndex, UnitIndex);
                UnitTargetLocations[UnitIndex] = HighLOD_Slots[SlotIndex];
            }
        }
        else if (LODLevel == EAILODLevel::Medium)
        {
            if (OccupiedMediumLODSlots.Num() < MediumLOD_Slots.Num())
            {
                const int32 SlotIndex = OccupiedMediumLODSlots.Num();
                OccupiedMediumLODSlots.Add(SlotIndex, UnitIndex);
                UnitTargetLocations[UnitIndex] = MediumLOD_Slots[SlotIndex];
            }
        }
    }
}

TArray<FVector> ASLBattleManager::CalculateCirclePositions(const FVector& Center, float Radius, int32 NumSlots) const
{
	TArray<FVector> Positions;
	if (NumSlots <= 0) return Positions;

	const float AngleStep = 360.0f / NumSlots;

	for (int32 i = 0; i < NumSlots; ++i)
	{
		const float Angle = FMath::DegreesToRadians(AngleStep * i);
		const float X = FMath::Cos(Angle) * Radius;
		const float Y = FMath::Sin(Angle) * Radius;
		Positions.Add(Center + FVector(X, Y, 0));
	}

	return Positions;
}

bool ASLBattleManager::AreEnemies(const FGenericTeamId& me, const FGenericTeamId& target, const bool bIsPlayer) const
{
	if (bIsPlayerOnly && bIsPlayer)
	{
		return true;
	}
	
	if (me == target)
	{
		return false;
	}

	if (me == FGenericTeamId::NoTeam || target == FGenericTeamId::NoTeam)
	{
		return false;
	}

	if (!bIsPlayer)
	{
		bool bTeamA_Managed = TeamUnitIndices.Contains(me);
		bool bTeamB_Managed = TeamUnitIndices.Contains(target);

		if (!bTeamA_Managed && !bTeamB_Managed)
		{
			return false;
		}
	}

	return me != target;
}

TArray<FBattleUnitInfo> ASLBattleManager::GetUnitsOfTeam(const FGenericTeamId& TeamId)
{
	TArray<FBattleUnitInfo> Result;
	if (const FTeamIndicesArrayWrapper* Wrapper = TeamUnitIndices.Find(TeamId))
	{
		for (const int32 Index : Wrapper->Indices)
		{
			Result.Add(GetUnitInfoByIndex(Index));
		}
	}
	return Result;
}

FBattleUnitInfo ASLBattleManager::GetUnitInfoByIndex(int32 Index) const
{
	FBattleUnitInfo Info;
	if (UnitActors.IsValidIndex(Index))
	{
		Info.Actor = UnitActors[Index];
		Info.TeamId = UnitTeamIDs[Index];
		Info.SourceSpawner = UnitSourceSpawners[Index];

		const int32 TargetIndex = UnitEngagedTargetIndices[Index];
		if (UnitActors.IsValidIndex(TargetIndex))
		{
			Info.CurrentEngagedTarget = UnitActors[TargetIndex];
		}
	}
	return Info;
}

void ASLBattleManager::StartNextGlobalWave()
{
	if (ManagedSpawners.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASLBattleManager: 관리할 웨이브 스포너 컴포넌트가 없습니다. 다음 웨이브를 시작할 수 없습니다."));
		return;
	}

	bool bAllCurrentWavesCompleted = true;
	for (const auto& Spawner : SpawnerWaveCompletionStatus)
	{
		if (Spawner.Key == nullptr || !Spawner.Value)
		{
			bAllCurrentWavesCompleted = false;
			break;
		}
	}

	if (!bAllCurrentWavesCompleted)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASLBattleManager: 모든 스포너 컴포넌트가 현재 웨이브를 완료하지 않아 다음 전역 웨이브를 시작할 수 없습니다."));
		return;
	}

	CurrentGlobalWaveNumber++;
	SpawnerWaveCompletionStatus.Empty();

	UE_LOG(LogTemp, Log, TEXT("ASLBattleManager: 다음 전역 웨이브 (%d) 시작을 지시합니다."), CurrentGlobalWaveNumber);

	bool bAnySpawnerHasNextWave = false;

	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			if (Spawner->StartWave(CurrentGlobalWaveNumber))
			{
				SpawnerWaveCompletionStatus.Add(Spawner, false);
				bAnySpawnerHasNextWave = true;
			}
			else
			{
				SpawnerWaveCompletionStatus.Add(Spawner, true);
				UE_LOG(LogTemp, Log, TEXT("ASLBattleManager: 스포너 '%s'는 모든 웨이브를 완료했습니다. (전역 웨이브 %d)"),
				       *Spawner->GetName(), CurrentGlobalWaveNumber -1);
			}
		}
	}

	if (!bAnySpawnerHasNextWave)
	{
		UE_LOG(LogTemp, Log, TEXT("ASLBattleManager: 모든 스포너 컴포넌트의 모든 웨이브가 최종적으로 완료되었습니다!"));
		EndBattle_Implementation(true);
	}
}

TArray<FBattleUnitInfo> ASLBattleManager::GetUnitsSpawnedBySpawner(ASLSwarmSpawner* Spawner) const
{
	TArray<FBattleUnitInfo> Result;
	if (!IsValid(Spawner)) return Result;

	for (int32 i = 0; i < UnitActors.Num(); ++i)
	{
		if (UnitSourceSpawners[i] == Spawner)
		{
			Result.Add(GetUnitInfoByIndex(i));
		}
	}
	return Result;
}

// Engage System
bool ASLBattleManager::RequestEngagementPermission(AActor* RequestingUnit, AActor* TargetActor)
{
	if (!RequestingUnit || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("RequestEngagementPermission: 유효하지 않은 유닛 또는 타겟입니다."));
		return false;
	}

	for (auto& EngagementPair : EngagedUnitsPerTarget)
	{
		if (EngagementPair.Value.EngagedUnits.Contains(RequestingUnit))
		{
			if (EngagementPair.Key == TargetActor)
			{
				UE_LOG(LogTemp, Log, TEXT("RequestEngagementPermission: %s는 이미 %s와 교전 중입니다."),
				       *RequestingUnit->GetName(), *TargetActor->GetName());
				return true;
			}
			UE_LOG(LogTemp, Log, TEXT("RequestEngagementPermission: %s가 기존 타겟과의 교전을 해제하고 새 타겟으로 전환합니다."),
			       *RequestingUnit->GetName());
			ReleaseEngagementPermission(RequestingUnit, EngagementPair.Key);
			break;
		}
	}

	// Race Condition
	int32& CurrentCountRef = TargetEngagementCounts.FindOrAdd(TargetActor, 0);
	if (CurrentCountRef < MaxEngagingUnitsPerTarget)
	{
		CurrentCountRef++;
		EngagedUnitsPerTarget.FindOrAdd(TargetActor).EngagedUnits.AddUnique(RequestingUnit);

		if (const int32* UnitIndexPtr = UnitIndexMap.Find(RequestingUnit))
		{
			if (const int32* TargetIndexPtr = UnitIndexMap.Find(TargetActor))
			{
				UnitEngagedTargetIndices[*UnitIndexPtr] = *TargetIndexPtr;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("교전 권한 승인: %s -> %s (현재 교전: %d/%d)"),
		       *RequestingUnit->GetName(), *TargetActor->GetName(),
		       CurrentCountRef, MaxEngagingUnitsPerTarget);

		return true;
	}

	return false;
}

void ASLBattleManager::ReleaseEngagementPermission(AActor* ReleasingUnit, AActor* TargetActor)
{
	if (!ReleasingUnit || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReleaseEngagementPermission: 유효하지 않은 유닛 또는 타겟입니다."));
		return;
	}

	if (int32* CountPtr = TargetEngagementCounts.Find(TargetActor))
	{
		*CountPtr = FMath::Max(0, *CountPtr - 1);

		if (*CountPtr == 0)
		{
			TargetEngagementCounts.Remove(TargetActor);
		}
	}

	if (FEngagedUnitsWrapper* EngagedUnits = EngagedUnitsPerTarget.Find(TargetActor))
	{
		EngagedUnits->EngagedUnits.Remove(ReleasingUnit);

		if (EngagedUnits->EngagedUnits.Num() == 0)
		{
			EngagedUnitsPerTarget.Remove(TargetActor);
		}
	}

	if (const int32* UnitIndexPtr = UnitIndexMap.Find(ReleasingUnit))
	{
		UnitEngagedTargetIndices[*UnitIndexPtr] = INDEX_NONE;
	}

	UE_LOG(LogTemp, Log, TEXT("교전 권한 해제: %s -> %s"),
	       *ReleasingUnit->GetName(), *TargetActor->GetName());
}

// Rebuild AI
void ASLBattleManager::ProcessSupportingAIReassignment()
{
	TArray<int32> SupportingAIIndices = FindSupportingAIs();
	if (SupportingAIIndices.Num() == 0)
	{
		return;
	}

	TArray<AActor*> AvailableTargets = FindTargetsWithOpenSlots();
	if (AvailableTargets.Num() == 0)
	{
		return;
	}

	int32 TargetIndex = 0;
	for (const int32 AIIndex : SupportingAIIndices)
	{
		if (TargetIndex >= AvailableTargets.Num())
		{
			break;
		}

		AActor* SelectedTarget = AvailableTargets[TargetIndex];
		AssignSupportingAIToTarget(AIIndex, SelectedTarget); // 인덱스로 호출

		TargetIndex = (TargetIndex + 1) % AvailableTargets.Num();
	}
}

TArray<int32> ASLBattleManager::FindSupportingAIs()
{
	TArray<int32> SupportingAIIndices;

	for (int32 i = 0; i < UnitActors.Num(); ++i)
	{
		const USLAICombatComponent* CombatComp = UnitCombatComponents[i];
		if (IsValid(CombatComp) && CombatComp->IsSupporting())
		{
			SupportingAIIndices.Add(i);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("지원 가능한 AI %d개 발견"), SupportingAIIndices.Num());
	return SupportingAIIndices;
}

TArray<AActor*> ASLBattleManager::FindTargetsWithOpenSlots()
{
	TArray<AActor*> AvailableTargets;

	for (const auto& EngagementPair : TargetEngagementCounts)
	{
		AActor* Target = EngagementPair.Key;
		const int32 CurrentEngagements = EngagementPair.Value;

		if (IsValid(Target) && CurrentEngagements < MaxEngagingUnitsPerTarget)
		{
			AvailableTargets.Add(Target);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("여분이 있는 타겟 %d개 발견"), AvailableTargets.Num());
	return AvailableTargets;
}

void ASLBattleManager::AssignSupportingAIToTarget(int32 SupportingAIIndex, AActor* Target)
{
	if (!UnitActors.IsValidIndex(SupportingAIIndex) || !IsValid(Target)) return;

	AActor* SupportingActor = UnitActors[SupportingAIIndex];
	if (USLAIStateComponent* StateComp = SupportingActor->FindComponentByClass<USLAIStateComponent>())
	{
		StateComp->StartSupportMovement(Target);
		UE_LOG(LogTemp, Log, TEXT("지원 AI %s를 타겟 %s로 이동 지시"), *SupportingActor->GetName(), *Target->GetName());
	}
}

int32 ASLBattleManager::GetCurrentEngagementCount(AActor* TargetActor) const
{
	if (const int32* CountPtr = TargetEngagementCounts.Find(TargetActor))
	{
		return *CountPtr;
	}
	return 0;
}

void ASLBattleManager::OnUnitDestroyed(AActor* DestroyedUnit)
{
	if (!DestroyedUnit)
	{
		return;
	}

	TArray<TObjectPtr<AActor>> TargetsToCleanup;

	for (auto& EngagementPair : EngagedUnitsPerTarget)
	{
		if (EngagementPair.Value.EngagedUnits.Contains(DestroyedUnit))
		{
			TargetsToCleanup.Add(EngagementPair.Key);
		}
	}

	for (TObjectPtr<AActor> Target : TargetsToCleanup)
	{
		ReleaseEngagementPermission(DestroyedUnit, Target);
	}

	UE_LOG(LogTemp, Log, TEXT("유닛 파괴로 인한 교전 권한 정리 완료: %s"), *DestroyedUnit->GetName());
}
