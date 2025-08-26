#include "SLBattleManager.h"

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
	PrimaryActorTick.TickInterval = 0.25f;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CurrentGlobalWaveNumber = 0;
}

void ASLBattleManager::BeginPlay()
{
	Super::BeginPlay();

	RegisteredUnits.Empty();
	TeamUnitIndices.Empty();
	TargetEngagementCounts.Empty();
	EngagedUnitsPerTarget.Empty();

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
	if (!IsValid(Actor)) return;

	for (const auto& Unit : RegisteredUnits)
	{
		if (Unit.Actor == Actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("유닛 %s는 이미 등록되어 있습니다."), *Actor->GetName());
			return;
		}
	}

	FGenericTeamId TeamId = FGenericTeamId::NoTeam;
	USLAIStateComponent* WarComp = nullptr;
	USLAITokenSystemComponent* TokenComp = nullptr;

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

	if (USLAIStateComponent* BattleComp = Actor->FindComponentByClass<USLAIStateComponent>())
	{
		WarComp = BattleComp;
	}

	FBattleUnitInfo NewUnit;
	NewUnit.Actor = Actor;
	NewUnit.TeamId = TeamId;
	NewUnit.bIsPlayer = bIsPlayer;
	NewUnit.SourceSpawner = SourceSpawner;

	if (IsValid(WarComp))
	{
		NewUnit.WarComponent = WarComp;
	}

	int32 NewIndex = RegisteredUnits.Add(NewUnit);
	RebuildTeamIndices();

	UE_LOG(LogTemp, Log, TEXT("배틀매니저: 유닛 등록됨: %s (팀: %d, 플레이어: %s, 스포너: %s)"),
	       *Actor->GetName(),
	       NewUnit.TeamId.GetId(),
	       bIsPlayer ? TEXT("예") : TEXT("아니오"),
	       IsValid(SourceSpawner) ? *SourceSpawner->GetName() : TEXT("없음"));
}

void ASLBattleManager::UnregisterUnit(AActor* Actor)
{
	if (!IsValid(Actor)) return;

	OnUnitDestroyed(Actor);

	int32 FoundIndex = INDEX_NONE;
	for (int32 i = 0; i < RegisteredUnits.Num(); ++i)
	{
		if (RegisteredUnits[i].Actor == Actor)
		{
			FoundIndex = i;
			break;
		}
	}

	if (FoundIndex != INDEX_NONE)
	{
		FBattleUnitInfo& UnitInfoToRemove = RegisteredUnits[FoundIndex];
		UnitInfoToRemove.CurrentEngagedTarget = nullptr;

		RegisteredUnits.RemoveAt(FoundIndex);
		RebuildTeamIndices();

		UE_LOG(LogTemp, Log, TEXT("배틀매니저: 유닛 등록 해제됨: %s"), *Actor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("배틀매니저: 등록되지 않은 유닛 %s에 대한 등록 해제 요청."), *Actor->GetName());
	}
}

void ASLBattleManager::RebuildTeamIndices()
{
	TeamUnitIndices.Empty();

	for (int32 i = 0; i < RegisteredUnits.Num(); i++)
	{
		FGenericTeamId TeamId = RegisteredUnits[i].TeamId;

		if (!TeamUnitIndices.Contains(TeamId))
		{
			TeamUnitIndices.Add(TeamId, FTeamIndicesArrayWrapper());
		}

		TeamUnitIndices[TeamId].Indices.Add(i);
	}
}

void ASLBattleManager::UpdateAILODs()
{
	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || RegisteredUnits.Num() == 0) return;

	const FVector PlayerLocation = PlayerPawn->GetActorLocation();

	TArray<FBattleUnitInfo*> MaxDistanceAIs;
	TArray<FBattleUnitInfo*> HighDistanceAIs;
	TArray<FBattleUnitInfo*> MediumDistanceAIs;
	TArray<FBattleUnitInfo*> LowDistanceAIs;

	for (FBattleUnitInfo& UnitInfo : RegisteredUnits)
	{
		if (!IsValid(UnitInfo.Actor)) continue;

		USLAILODComponent* LODComponent = UnitInfo.Actor->FindComponentByClass<USLAILODComponent>();
		if (!LODComponent) continue;

		const EAILODLevel CalculatedLOD = LODComponent->CalculateLODLevel();

		switch (CalculatedLOD)
		{
		case EAILODLevel::Max:
			MaxDistanceAIs.Add(&UnitInfo);
			break;
		case EAILODLevel::High:
			HighDistanceAIs.Add(&UnitInfo);
			break;
		case EAILODLevel::Medium:
			MediumDistanceAIs.Add(&UnitInfo);
			break;
		case EAILODLevel::Low:
			LowDistanceAIs.Add(&UnitInfo);
			break;
		case EAILODLevel::Culled:
			LODComponent->SetLODLevel(EAILODLevel::Culled);
			break;
		}
	}

	MaxDistanceAIs.Sort([PlayerLocation](const FBattleUnitInfo& A, const FBattleUnitInfo& B)
	{
		return FVector::DistSquared(A.Actor->GetActorLocation(), PlayerLocation) < FVector::DistSquared(
			B.Actor->GetActorLocation(), PlayerLocation);
	});
	
	for (int32 i = 0; i < MaxDistanceAIs.Num(); ++i)
	{
		USLAILODComponent* LODComponent = MaxDistanceAIs[i]->Actor->FindComponentByClass<USLAILODComponent>();
		if (i < LODBudget.MaxLODCount)
		{
			LODComponent->SetLODLevel(EAILODLevel::Max);
		}
		else
		{
			LODComponent->SetLODLevel(EAILODLevel::High);
		}
	}

	HighDistanceAIs.Sort([PlayerLocation](const FBattleUnitInfo& A, const FBattleUnitInfo& B)
	{
		return FVector::DistSquared(A.Actor->GetActorLocation(), PlayerLocation) < FVector::DistSquared(
			B.Actor->GetActorLocation(), PlayerLocation);
	});
	for (int32 i = 0; i < HighDistanceAIs.Num(); ++i)
	{
		USLAILODComponent* LODComponent = HighDistanceAIs[i]->Actor->FindComponentByClass<USLAILODComponent>();
		if (i < LODBudget.HighLODCount)
		{
			LODComponent->SetLODLevel(EAILODLevel::High);
		}
		else
		{
			LODComponent->SetLODLevel(EAILODLevel::Medium);
		}
	}

	MediumDistanceAIs.Sort([PlayerLocation](const FBattleUnitInfo& A, const FBattleUnitInfo& B)
	{
		return FVector::DistSquared(A.Actor->GetActorLocation(), PlayerLocation) < FVector::DistSquared(
			B.Actor->GetActorLocation(), PlayerLocation);
	});
	
	for (int32 i = 0; i < MediumDistanceAIs.Num(); ++i)
	{
		USLAILODComponent* LODComponent = MediumDistanceAIs[i]->Actor->FindComponentByClass<USLAILODComponent>();
		if (i < LODBudget.MediumLODCount)
		{
			LODComponent->SetLODLevel(EAILODLevel::Medium);
		}
		else
		{
			LODComponent->SetLODLevel(EAILODLevel::Low);
		}
	}

	for (FBattleUnitInfo* UnitInfo : LowDistanceAIs)
	{
		UnitInfo->Actor->FindComponentByClass<USLAILODComponent>()->SetLODLevel(EAILODLevel::Low);
	}
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

	if (TeamUnitIndices.Contains(TeamId))
	{
		const FTeamIndicesArrayWrapper& Wrapper = TeamUnitIndices[TeamId];
		for (const int32 Index : Wrapper.Indices)
		{
			if (RegisteredUnits.IsValidIndex(Index))
			{
				Result.Add(RegisteredUnits[Index]);
			}
		}
	}

	return Result;
}

TArray<FBattleUnitInfo> ASLBattleManager::GetEnemiesOfTeam(const FGenericTeamId& TeamId)
{
	TArray<FBattleUnitInfo> Result;

	for (const auto& Unit : RegisteredUnits)
	{
		if (AreEnemies(TeamId, Unit.TeamId, false))
		{
			Result.Add(Unit);
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
	if (!IsValid(Spawner))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetUnitsSpawnedBySpawner: 유효하지 않은 스포너가 전달되었습니다."));
		return Result;
	}

	for (const FBattleUnitInfo& Unit : RegisteredUnits)
	{
		if (Unit.SourceSpawner == Spawner)
		{
			Result.Add(Unit);
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

void ASLBattleManager::StartBattle()
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

void ASLBattleManager::EndBattle()
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

	RegisteredUnits.Empty();
	TeamUnitIndices.Empty();
	TargetEngagementCounts.Empty();
	EngagedUnitsPerTarget.Empty();
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

		if (FBattleUnitInfo* UnitInfo = FindUnitInfo(RequestingUnit))
		{
			UnitInfo->CurrentEngagedTarget = TargetActor;
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

	if (FBattleUnitInfo* UnitInfo = FindUnitInfo(ReleasingUnit))
	{
		UnitInfo->CurrentEngagedTarget = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("교전 권한 해제: %s -> %s"),
	       *ReleasingUnit->GetName(), *TargetActor->GetName());
}

// Rebuild AI
void ASLBattleManager::ProcessSupportingAIReassignment()
{
	// 지원 중인 AI들 찾기
	TArray<FBattleUnitInfo> SupportingAIs = FindSupportingAIs();
	if (SupportingAIs.Num() == 0)
	{
		return;
	}

	// 여분이 있는 타겟들 찾기
	TArray<AActor*> AvailableTargets = FindTargetsWithOpenSlots();
	if (AvailableTargets.Num() == 0)
	{
		return;
	}

	// AI들을 타겟에 배치
	int32 TargetIndex = 0;
	for (FBattleUnitInfo& SupportingAI : SupportingAIs)
	{
		if (TargetIndex >= AvailableTargets.Num())
		{
			break;
		}

		AActor* SelectedTarget = AvailableTargets[TargetIndex];
		AssignSupportingAIToTarget(SupportingAI, SelectedTarget);

		TargetIndex = (TargetIndex + 1) % AvailableTargets.Num();
	}
}

TArray<FBattleUnitInfo> ASLBattleManager::FindSupportingAIs()
{
	TArray<FBattleUnitInfo> SupportingAIs;

	for (FBattleUnitInfo& Unit : RegisteredUnits)
	{
		if (!IsValid(Unit.Actor)) continue;

		if (const USLAICombatComponent* CombatComp = Unit.Actor->FindComponentByClass<USLAICombatComponent>())
		{
			if (CombatComp->IsSupporting())
			{
				SupportingAIs.Add(Unit);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("지원 가능한 AI %d개 발견"), SupportingAIs.Num());
	return SupportingAIs;
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

void ASLBattleManager::AssignSupportingAIToTarget(const FBattleUnitInfo& SupportingAI, AActor* Target)
{
	if (!IsValid(SupportingAI.Actor) || !IsValid(Target)) return;

	if (USLAIStateComponent* StateComp = SupportingAI.Actor->FindComponentByClass<USLAIStateComponent>())
	{
		StateComp->StartSupportMovement(Target);
		UE_LOG(LogTemp, Log, TEXT("지원 AI %s를 타겟 %s로 이동 지시"),
		       *SupportingAI.Actor->GetName(), *Target->GetName());
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

// 헬퍼 함수 - 유닛 정보 찾기
FBattleUnitInfo* ASLBattleManager::FindUnitInfo(AActor* Unit)
{
	for (FBattleUnitInfo& UnitInfo : RegisteredUnits)
	{
		if (UnitInfo.Actor == Unit)
		{
			return &UnitInfo;
		}
	}
	return nullptr;
}
