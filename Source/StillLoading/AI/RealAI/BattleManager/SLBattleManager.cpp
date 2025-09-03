#include "SLBattleManager.h"

#include "AI/RealAI/SLMonsterAICharacterBase.h"
#include "AI/RealAI/Component/SLAICombatComponent.h"
#include "AI/RealAI/Component/SLAILODComponent.h"
#include "AI/RealAI/Component/SLAIStateComponent.h"
#include "AI/RealAI/Spawner/SLSwarmSpawner.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ASLBattleManager::ASLBattleManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.2f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CurrentGlobalWaveNumber = 0;
}

void ASLBattleManager::StartBattle_Implementation()
{
	if (ManagedSpawners.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASLBattleManager: 관리할 스포너가 없습니다. 전투를 시작할 수 없습니다."));
		return;
	}

	CurrentGlobalWaveNumber = 0;
	SpawnerWaveCompletionStatus.Empty();

	UE_LOG(LogTemp, Log, TEXT("ASLBattleManager: 전투 시작! 첫 전역 웨이브 (%d)를 지시합니다."), CurrentGlobalWaveNumber);

	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->StartWave(CurrentGlobalWaveNumber);
			SpawnerWaveCompletionStatus.Add(Spawner, false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ASLBattleManager: ManagedSpawners에 유효하지 않은 스포너가 있습니다."));
		}
	}
}

void ASLBattleManager::EndBattle_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("SLBattleManager: 전투 종료!"));
	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->StopWaveSpawning();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SLBattleManager: ManagedSpawners에 유효하지 않은 스포너가 있습니다. 종료할 수 없습니다."));
		}
	}

	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->CleanupPool();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SLBattleManager: 등록된 스포너 중 유효하지 않은 것이 있습니다."));
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

void ASLBattleManager::BeginPlay()
{
	Super::BeginPlay();

	//TODO::추후에 가져오는 방식 변경 필요
	PrimaryTarget = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

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

	BindToSpawnerEvents();
	InitializeAISupportingMode();

#if WITH_EDITOR
	if (GetWorld())
	{
		TArray<FColor> DebugColors;
		DebugColors.Add(FColor::Red); // 스포너 0번
		DebugColors.Add(FColor::Green); // 스포너 1번
		DebugColors.Add(FColor::Blue);
		DebugColors.Add(FColor::Yellow);
		DebugColors.Add(FColor::Cyan);
		DebugColors.Add(FColor::Orange);
		DebugColors.Add(FColor::Purple);
		DebugColors.Add(FColor::White);

		int32 SpawnerIndex = 0;

		for (const FSpawnerTargetPoints& SpawnerData : SpawnerTargetPointData)
		{
			if (!IsValid(SpawnerData.Spawner))
			{
				SpawnerIndex++;
				continue;
			}

			FColor CurrentColor = DebugColors.IsValidIndex(SpawnerIndex) ? DebugColors[SpawnerIndex] : FColor::White;

			DrawDebugSphere(GetWorld(), SpawnerData.Spawner->GetActorLocation(), 70.f, 12, CurrentColor, true, -1.f, 0,
			                4.f);
			DrawDebugString(GetWorld(), SpawnerData.Spawner->GetActorLocation() + FVector(0, 0, 100),
			                FString::Printf(TEXT("Spawner %d"), SpawnerIndex), nullptr, CurrentColor, -1.f, true);

			FVector PreviousTargetLocation = SpawnerData.Spawner->GetActorLocation();

			for (int32 i = 0; i < SpawnerData.TargetPoints.Num(); ++i)
			{
				ATargetPoint* TargetPoint = SpawnerData.TargetPoints[i];
				if (IsValid(TargetPoint))
				{
					FVector CurrentTargetLocation = TargetPoint->GetActorLocation();

					DrawDebugSphere(GetWorld(), CurrentTargetLocation, 100.f, 12, CurrentColor, true, -1.f, 0, 3.f);
					DrawDebugLine(GetWorld(), PreviousTargetLocation, CurrentTargetLocation, CurrentColor, true, -1.f,
					              0, 2.f);
					DrawDebugString(GetWorld(), CurrentTargetLocation + FVector(0, 0, 50), FString::FromInt(i), nullptr,
					                CurrentColor, -1.f, true);

					PreviousTargetLocation = CurrentTargetLocation;
				}
			}
			SpawnerIndex++;
		}
	}
#endif
}

void ASLBattleManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bUseLODSystem) return;
	UpdateAILODs();

	if (!bIsPlayerOnly) return;
	UpdateEncounterPositions();
}

void ASLBattleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndBattle();

	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->OnUnitSpawned.RemoveDynamic(this, &ASLBattleManager::OnSpawnerUnitSpawnedHandler);
			Spawner->OnUnitReturnedToPool.RemoveDynamic(this, &ASLBattleManager::OnSpawnerUnitReturnedToPoolHandler);
			Spawner->OnUnitActuallyDestroyed.RemoveDynamic(
				this, &ASLBattleManager::OnSpawnerUnitActuallyDestroyedHandler);
			Spawner->OnWaveCompletedBySpawner.RemoveDynamic(this, &ASLBattleManager::HandleWaveCompleted);
			Spawner->OnAllWavesCompletedBySpawner.RemoveDynamic(this, &ASLBattleManager::HandleAllWavesCompleted);
		}
	}

	Super::EndPlay(EndPlayReason);
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

		if (AreEnemies(MyTeamID, UnitTeamIDs[i], false))
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
	for (ASLSwarmSpawner* Spawner : ManagedSpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->SetCachedBattleManager(this);
			Spawner->OnWaveCompletedBySpawner.AddDynamic(this, &ASLBattleManager::HandleWaveCompleted);
			Spawner->OnAllWavesCompletedBySpawner.AddDynamic(this, &ASLBattleManager::HandleAllWavesCompleted);
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
		RegisterUnit(SpawnedUnit, false, SourceSpawner);
	}

	ACharacter* SpawnedCharacter = Cast<ACharacter>(SpawnedUnit);
	if (IsValid(SpawnedCharacter))
	{
		if (USLAIStateComponent* WarComp = SpawnedCharacter->FindComponentByClass<USLAIStateComponent>())
		{
			WarComp->ActivateAndMoveToInitialTarget(0);
			UE_LOG(LogTemp, Log, TEXT("SLBattleManager: 스폰된 유닛 '%s' 초기 타겟 포인트 0번으로 이동 시작."),
			       *SpawnedCharacter->GetName());
		}
	}
}

void ASLBattleManager::OnSpawnerUnitReturnedToPoolHandler(AActor* ReturnedUnit, ASLSwarmSpawner* SourceSpawner)
{
	if (IsValid(ReturnedUnit))
	{
		UnregisterUnit(ReturnedUnit);
	}
}

void ASLBattleManager::OnSpawnerUnitActuallyDestroyedHandler(AActor* DestroyedActor)
{
	if (IsValid(DestroyedActor))
	{
		UnregisterUnit(DestroyedActor);
	}
}

FVector ASLBattleManager::GetNextTargetPointLocationForSpawner(ASLSwarmSpawner* ForSpawner,
                                                               int32& CurrentTargetIndex) const
{
	if (!IsValid(ForSpawner))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetNextTargetPointLocationForSpawner: 유효하지 않은 스포너가 전달되었습니다."));
		return FVector::ZeroVector;
	}

	for (const FSpawnerTargetPoints& SpawnerData : SpawnerTargetPointData)
	{
		if (SpawnerData.Spawner == ForSpawner)
		{
			const int32 NumTargetPoints = SpawnerData.TargetPoints.Num();

			if (NumTargetPoints == 0 || !SpawnerData.TargetPoints.IsValidIndex(CurrentTargetIndex)
				|| SpawnerData.TargetPoints[CurrentTargetIndex] == nullptr)
			{
				UE_LOG(LogTemp, Warning,
				       TEXT(
					       "GetNextTargetPointLocationForSpawner: 스포너 '%s'의 타겟 포인트 인덱스 %d에 유효한 타겟이 없거나, 타겟 포인트가 없습니다. 순회 종료."
				       ),
				       *ForSpawner->GetName(), CurrentTargetIndex);
				return FVector::ZeroVector;
			}

			FVector Location = SpawnerData.TargetPoints[CurrentTargetIndex]->GetActorLocation();

			if (NumTargetPoints != 1)
			{
				CurrentTargetIndex = (CurrentTargetIndex + 1) % NumTargetPoints;
			}

			return Location;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GetNextTargetPointLocationForSpawner: 스포너 '%s'에 대한 타겟 포인트 데이터를 찾을 수 없습니다."),
	       *ForSpawner->GetName());
	return FVector::ZeroVector;
}

// Unit
void ASLBattleManager::RegisterUnit(AActor* Actor, bool bIsPlayer, ASLSwarmSpawner* SourceSpawner)
{
	if (!IsValid(Actor) || UnitIndexMap.Contains(Actor))
	{
		if (Actor) UE_LOG(LogTemp, Warning, TEXT("유닛 %s는 이미 등록되어 있습니다."), *Actor->GetName());
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

	UE_LOG(LogTemp, Log, TEXT("배틀매니저: 유닛 등록됨: %s (팀: %d, 스포너: %s)"),
		  *Actor->GetName(),
		  TeamId.GetId(),
		  IsValid(SourceSpawner) ? *SourceSpawner->GetName() : TEXT("없음"));
}

void ASLBattleManager::UnregisterUnit(AActor* Actor)
{
	if (!IsValid(Actor)) return;

	OnUnitDestroyed(Actor);

	const int32* IndexPtr = UnitIndexMap.Find(Actor);
	if (!IndexPtr) return;

	const int32 IndexToRemove = *IndexPtr;
	const int32 LastIndex = UnitActors.Num() - 1;

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

	UnitIndexMap.Remove(Actor);
}

void ASLBattleManager::UpdateAILODs()
{
    const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn || UnitActors.Num() == 0) return;

    const FVector PlayerLocation = PlayerPawn->GetActorLocation();
    const int32 NumUnits = UnitActors.Num();

    for (int32 i = 0; i < NumUnits; ++i)
    {
        if (UnitActors[i])
        {
            UnitLocations[i] = UnitActors[i]->GetActorLocation();
        }
    }

    TArray<EAILODLevel> FinalLODs;
    FinalLODs.SetNum(NumUnits);
    TArray<int32> UnitIndicesByLOD[5];

    for (int32 i = 0; i < NumUnits; ++i)
    {
        const float DistanceSquared = FVector::DistSquared(UnitLocations[i], PlayerLocation);
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
            return FVector::DistSquared(UnitLocations[A], PlayerLocation) < FVector::DistSquared(UnitLocations[B], PlayerLocation);
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
    // 1. Max LOD를 위한 슬롯 준비 및 정리 (기존 슬롯 점유자 유지)
    // ==================================================================
    const TArray<FVector> AttackSlots = CalculateCirclePositions(PlayerLocation, PressurerCircleRadius, LODBudget.MaxLODCount);
    
    // 유효하지 않은(죽거나 Max LOD가 아니게 된) 유닛이 차지한 슬롯을 비웁니다.
    if (AttackSlots.Num() > 0)
    {
        TArray<int32> SlotsToClear;
        for (auto& Elem : OccupiedAttackSlots)
        {
            const int32 UnitIndex = Elem.Value;
            if (!UnitActors.IsValidIndex(UnitIndex) || !IsValid(UnitLODComponents[UnitIndex]) || UnitLODComponents[UnitIndex]->GetCurrentLODLevel() != EAILODLevel::Max)
            {
                SlotsToClear.Add(Elem.Key);
            }
        }
        for (const int32 SlotIndex : SlotsToClear)
        {
            OccupiedAttackSlots.Remove(SlotIndex);
        }
    }

    // ==================================================================
    // 2. 모든 유닛을 LOD 레벨에 따라 분류
    // ==================================================================
    TArray<int32> MaxLOD_Candidates, HighLOD_Indices, MediumLOD_Indices;
    for (int32 i = 0; i < UnitActors.Num(); ++i)
    {
        if (IsValid(UnitLODComponents[i]))
        {
            EAILODLevel LODLevel = UnitLODComponents[i]->GetCurrentLODLevel();
            if (LODLevel == EAILODLevel::Max)
            {
                // 이 유닛이 아직 슬롯을 점유하지 않았다면, 새로운 후보가 됩니다.
                if (!OccupiedAttackSlots.FindKey(i))
                {
                    MaxLOD_Candidates.Add(i);
                }
            }
            else if (LODLevel == EAILODLevel::High)
            {
                HighLOD_Indices.Add(i);
            }
            else if (LODLevel == EAILODLevel::Medium)
            {
                MediumLOD_Indices.Add(i);
            }
            else
            {
                // Max, High, Medium이 아닌 유닛의 타겟 위치는 초기화합니다.
                // 단, 이미 슬롯을 점유한 Max 유닛은 제외해야 하므로, 점유 상태를 확인합니다.
                if (!OccupiedAttackSlots.FindKey(i))
                {
                    UnitTargetLocations[i] = FVector::ZeroVector;
                }
            }
        }
    }

    // ==================================================================
    // 3. Max LOD 처리 (새로운 후보에게 빈 슬롯 우선 할당)
    // ==================================================================
    if (MaxLOD_Candidates.Num() > 0 && AttackSlots.Num() > 0)
    {
        // 후보들을 가까운 순으로 정렬
        MaxLOD_Candidates.Sort([this, &PlayerLocation](const int32& A, const int32& B){
            return FVector::DistSquared(UnitLocations[A], PlayerLocation) < FVector::DistSquared(UnitLocations[B], PlayerLocation);
        });

        // 정렬된 후보들에게 비어있는 슬롯 할당
        for (const int32 CandidateIndex : MaxLOD_Candidates)
        {
            if (OccupiedAttackSlots.Num() >= AttackSlots.Num()) break;
            for (int32 SlotIndex = 0; SlotIndex < AttackSlots.Num(); ++SlotIndex)
            {
                if (!OccupiedAttackSlots.Contains(SlotIndex))
                {
                    OccupiedAttackSlots.Add(SlotIndex, CandidateIndex);
                    break;
                }
            }
        }
    }

    // 최종적으로 점유된 모든 Max LOD 슬롯의 위치를 갱신
    for (auto& Elem : OccupiedAttackSlots)
    {
        const int32 SlotIndex = Elem.Key;
        const int32 UnitIndex = Elem.Value;
        if (AttackSlots.IsValidIndex(SlotIndex))
        {
            FVector Jitter = FVector(FMath::RandRange(-75.f, 75.f), FMath::RandRange(-75.f, 75.f), 0.f);
            UnitTargetLocations[UnitIndex] = AttackSlots[SlotIndex] + Jitter;
        }
    }
    
    // ==================================================================
    // 4. High LOD 처리 (기존 로직 유지)
    // ==================================================================
    if (HighLOD_Indices.Num() > 0)
    {
        const TArray<FVector> HighLOD_Slots = CalculateCirclePositions(PlayerLocation, PressurerCircleRadius, LODBudget.HighLODCount);
        if (HighLOD_Slots.Num() > 0)
        {
            for (int32 i = 0; i < HighLOD_Indices.Num(); ++i)
            {
                const int32 UnitIndex = HighLOD_Indices[i];
                const int32 SlotIndex = i % HighLOD_Slots.Num();
                UnitTargetLocations[UnitIndex] = HighLOD_Slots[SlotIndex];
            }
        }
    }

    // ==================================================================
    // 5. Medium LOD 처리 (기존 로직 유지)
    // ==================================================================
    if (MediumLOD_Indices.Num() > 0)
    {
        const TArray<FVector> MediumLOD_Slots = CalculateCirclePositions(PlayerLocation, MediumCircleRadius, LODBudget.MediumLODCount);
        if (MediumLOD_Slots.Num() > 0)
        {
            for (int32 i = 0; i < MediumLOD_Indices.Num(); ++i)
            {
                const int32 UnitIndex = MediumLOD_Indices[i];
                const int32 SlotIndex = i % MediumLOD_Slots.Num();
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

TArray<FBattleUnitInfo> ASLBattleManager::GetEnemiesOfTeam(const FGenericTeamId& TeamId)
{
	TArray<FBattleUnitInfo> Result;
	for (int32 i = 0; i < UnitActors.Num(); ++i)
	{
		if (AreEnemies(TeamId, UnitTeamIDs[i], false))
		{
			Result.Add(GetUnitInfoByIndex(i));
		}
	}
	return Result;
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

	CurrentGlobalWaveNumber++; // 다음 전역 웨이브 번호 증가
	SpawnerWaveCompletionStatus.Empty(); // 다음 웨이브를 위해 상태 초기화

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
		EndBattle();
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

void ASLBattleManager::HandleWaveCompleted(int32 WaveNumber, ASLSwarmSpawner* CompletedSpawner)
{
	if (SpawnerWaveCompletionStatus.Contains(CompletedSpawner))
	{
		SpawnerWaveCompletionStatus[CompletedSpawner] = true;
	}
}

void ASLBattleManager::HandleAllWavesCompleted(ASLSwarmSpawner* CompletedSpawner)
{
	if (SpawnerWaveCompletionStatus.Contains(CompletedSpawner))
	{
		SpawnerWaveCompletionStatus[CompletedSpawner] = true;
		UE_LOG(LogTemp, Log, TEXT("ASLBattleManager: 스포너 '%s'가 자신에게 할당된 모든 웨이브를 완료했습니다."),
		       *CompletedSpawner->GetName());
	}
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
