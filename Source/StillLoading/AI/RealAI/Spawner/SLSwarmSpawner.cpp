#include "SLSwarmSpawner.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "AI/RealAI/SLMonsterAICharacterBase.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "AI/RealAI/Component/SLAIStateComponent.h"
#include "AI/RealAI/Component/SLWaveSpawnerComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ASLSwarmSpawner::ASLSwarmSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	RootComponent = SpawnBox;
	SpawnBox->SetBoxExtent(FVector(500.f, 500.f, 100.f));
	SpawnBox->SetCollisionProfileName(TEXT("NoCollision"));
	SpawnBox->bHiddenInGame = true;
	SpawnBox->SetVisibility(true);

	WaveSpawnerComponent = CreateDefaultSubobject<USLWaveSpawnerComponent>(TEXT("WaveSpawnerComp"));
}

void ASLSwarmSpawner::SetCachedBattleManager(ASLBattleManager* NewBattleManager)
{
	if (IsValid(NewBattleManager) && !CachedBattleManager.IsValid())
	{
		CachedBattleManager = NewBattleManager;
		UE_LOG(LogTemp, Log, TEXT("SwarmSpawner: BattleManager 캐시됨."));
	}
}

void ASLSwarmSpawner::BeginPlay()
{
	Super::BeginPlay();

	ObjectPool.Empty();

	if (IsValid(WaveSpawnerComponent))
	{
		WaveSpawnerComponent->OnWaveCompleted.AddDynamic(this, &ASLSwarmSpawner::HandleInternalWaveCompleted);
		WaveSpawnerComponent->OnAllWavesCompleted.AddDynamic(this, &ASLSwarmSpawner::HandleInternalAllWavesCompleted);

		if (GetWorld() && GetWorld()->IsGameWorld())
		{
			InitializeObjectPool(WaveSpawnerComponent->GetAllWaveCompositions());

			GetWorld()->GetTimerManager().SetTimer(
				PoolExpansionTimer,
				this,
				&ASLSwarmSpawner::ExpandPoolIncrementally,
				0.05f,
				true
			);
		}
	}
}

void ASLSwarmSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		CleanupPool();
	}

	if (IsValid(WaveSpawnerComponent))
	{
		WaveSpawnerComponent->OnWaveCompleted.RemoveDynamic(this, &ASLSwarmSpawner::HandleInternalWaveCompleted);
		WaveSpawnerComponent->OnAllWavesCompleted.
		                      RemoveDynamic(this, &ASLSwarmSpawner::HandleInternalAllWavesCompleted);
	}

	Super::EndPlay(EndPlayReason);
}

void ASLSwarmSpawner::ReturnAllActiveUnitsToPool()
{
	TArray<FPooledUnit> ActiveUnitsCopy = ObjectPool;

	for (FPooledUnit& PoolEntry : ActiveUnitsCopy)
	{
		if (IsValid(PoolEntry.Character) && PoolEntry.bInUse)
		{
			ReturnUnitToPool(PoolEntry.Character);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("SwarmSpawner '%s': 모든 활성 유닛을 풀로 반환했습니다."), *GetName());
}

FVector ASLSwarmSpawner::GetNextTargetPointLocation(AActor* Unit, int32& CurrentTargetIndex)
{
	UE_LOG(LogTemp, Error, TEXT("타겟 인덱스 %d"), CurrentTargetIndex);
	
	if (PatrolPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("'%s' 스포너에 순찰 지점(PatrolPoints)이 등록되지 않았습니다."), *GetName());
		return GetActorLocation();
	}

	if (!PatrolPoints.IsValidIndex(CurrentTargetIndex) || !IsValid(PatrolPoints[CurrentTargetIndex]))
	{
		UE_LOG(LogTemp, Warning, TEXT("'%s' 스포너의 순찰 지점 인덱스 %d가 유효하지 않습니다. 인덱스를 0으로 초기화합니다."), *GetName(),
		       CurrentTargetIndex);
		CurrentTargetIndex = 0;
	}

	const FVector Location = PatrolPoints[CurrentTargetIndex]->GetActorLocation();
	CurrentTargetIndex = (CurrentTargetIndex + 1) % PatrolPoints.Num();

	USLAIStateComponent* StateComp = Unit->FindComponentByClass<USLAIStateComponent>();
	if (IsValid(StateComp))
	{
		StateComp->SetCurrentTargetIndex(CurrentTargetIndex);
	}

	UE_LOG(LogTemp, Error, TEXT("다음 타겟 인덱스 %d"), CurrentTargetIndex);

	return Location;
}

bool ASLSwarmSpawner::StartWave(int32 WaveIndex)
{
	if (IsValid(WaveSpawnerComponent))
	{
		return WaveSpawnerComponent->StartWaveByIndex(WaveIndex);
	}
	return false;
}

void ASLSwarmSpawner::StopWaveSpawning()
{
	if (IsValid(WaveSpawnerComponent))
	{
		WaveSpawnerComponent->StopWaveSpawning();
	}

	ReturnAllActiveUnitsToPool();
}

void ASLSwarmSpawner::HandleInternalWaveCompleted(int32 WaveNumber, USLWaveSpawnerComponent* CompletedComp)
{
	OnWaveCompletedBySpawner.Broadcast(WaveNumber, this);
}

void ASLSwarmSpawner::HandleInternalAllWavesCompleted(USLWaveSpawnerComponent* CompletedComp)
{
	OnAllWavesCompletedBySpawner.Broadcast(this);
}

void ASLSwarmSpawner::ExpandPoolIncrementally()
{
	if (!CurrentAsyncRequest.IsValid() && !SpawnRequestQueue.IsEmpty())
	{
		CurrentAsyncRequest = MakeShared<FAsyncSpawnRequest>();
		SpawnRequestQueue.Dequeue(*CurrentAsyncRequest);

		// 램에 미리 넣어두는 과정, 완료 되면 콜백 함수 호출 (느린 IO작업 미리 선행)
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		CurrentAsyncRequest->StreamableHandle = StreamableManager.RequestAsyncLoad(
			CurrentAsyncRequest->UnitClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ASLSwarmSpawner::OnUnitClassLoaded)
		);
		return;
	}

	if (!CurrentAsyncRequest.IsValid() && SpawnRequestQueue.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(PoolExpansionTimer);
		UE_LOG(LogTemp, Log, TEXT("SwarmSpawner: 객체 풀 비동기 스폰 완료. 총 %d 유닛 풀링됨."), ObjectPool.Num());
	}
}

void ASLSwarmSpawner::OnUnitClassLoaded()
{
	if (!CurrentAsyncRequest.IsValid() || !CurrentAsyncRequest->StreamableHandle.IsValid()) return;

	if (CurrentAsyncRequest->StreamableHandle->HasLoadCompleted())
	{
		TSubclassOf<ACharacter> LoadedClass(Cast<UClass>(CurrentAsyncRequest->StreamableHandle->GetLoadedAsset()));
		if (LoadedClass)
		{
			constexpr int32 MaxSpawnThisFrame = 2;
			const int32 NumToSpawn = FMath::Min(CurrentAsyncRequest->Count, MaxSpawnThisFrame);
			ExpandPool(LoadedClass, NumToSpawn);

			CurrentAsyncRequest->Count -= NumToSpawn;
		}
	}

	if (CurrentAsyncRequest->Count <= 0)
	{
		CurrentAsyncRequest.Reset();
	}
}

void ASLSwarmSpawner::InitializeObjectPool(const TArray<FSimpleSpawnComposition>& AllCompositionsToPool)
{
	ObjectPool.Empty();
	SpawnRequestQueue.Empty();

	TMap<TSoftClassPtr<ACharacter>, int32> RequiredUnitsPerClass;
	for (const FSimpleSpawnComposition& Composition : AllCompositionsToPool)
	{
		if (IsValid(Composition.UnitClass))
		{
			RequiredUnitsPerClass.FindOrAdd(Composition.UnitClass.Get()) += Composition.SpawnCount;
		}
	}

	int32 TotalRequired = 0;
	for (auto const& Elem : RequiredUnitsPerClass)
	{
		TotalRequired += Elem.Value;
	}
	if (TotalRequired < InitialPoolSize && RequiredUnitsPerClass.Num() > 0)
	{
		const TSoftClassPtr<ACharacter> DefaultUnitClass = RequiredUnitsPerClass.CreateConstIterator()->Key;
		RequiredUnitsPerClass.FindOrAdd(DefaultUnitClass) += (InitialPoolSize - TotalRequired);
	}

	for (auto const& Elem : RequiredUnitsPerClass)
	{
		if (Elem.Value > 0)
		{
			SpawnRequestQueue.Enqueue({Elem.Key, Elem.Value, nullptr});
		}
	}
}

ACharacter* ASLSwarmSpawner::GetPooledUnit(TSubclassOf<ACharacter> UnitClass)
{
	for (FPooledUnit& PoolEntry : ObjectPool)
	{
		if (!PoolEntry.bInUse && PoolEntry.UnitClass == UnitClass && IsValid(PoolEntry.Character))
		{
			PoolEntry.bInUse = true;
			PoolEntry.Character->SetActorHiddenInGame(false);
			PoolEntry.Character->SetActorEnableCollision(true);
			return PoolEntry.Character;
		}
	}

	if (bExpandPoolIfNeeded && ObjectPool.Num() < MaxPoolSize)
	{
		ExpandPool(UnitClass, 5);

		for (FPooledUnit& PoolEntry : ObjectPool)
		{
			if (!PoolEntry.bInUse && PoolEntry.UnitClass == UnitClass && IsValid(PoolEntry.Character))
			{
				PoolEntry.bInUse = true;
				PoolEntry.Character->SetActorHiddenInGame(false);
				PoolEntry.Character->SetActorEnableCollision(true);
				return PoolEntry.Character;
			}
		}
	}

	return nullptr;
}

void ASLSwarmSpawner::ReturnUnitToPool(ACharacter* Unit)
{
	if (!IsValid(Unit)) return;

	for (FPooledUnit& PoolEntry : ObjectPool)
	{
		if (PoolEntry.Character == Unit)
		{
			PoolEntry.bInUse = false;

			if (USLAIStateComponent* StateComp = Unit->FindComponentByClass<USLAIStateComponent>())
			{
				StateComp->DeactivateAndReset();
			}

			Unit->SetActorHiddenInGame(true);
			Unit->SetActorEnableCollision(false);
			Unit->SetActorTickEnabled(false);

			if (AController* Controller = Unit->GetController())
			{
				Controller->StopMovement();
				Controller->SetActorTickEnabled(false);
			}

			Unit->SetActorLocation(GetActorLocation() + FVector(0, 0, -10000));

			OnUnitReturnedToPool.Broadcast(Unit, this);

			if (USLWaveSpawnerComponent* WaveComp = FindComponentByClass<USLWaveSpawnerComponent>())
			{
				if (WaveComp->IsInfiniteRespawnMode())
				{
					WaveComp->OnUnitReturnedToPool(Unit->GetClass());
				}
			}

			UE_LOG(LogTemp, Log, TEXT("SwarmSpawner: 유닛 '%s' 풀에 반환."), *Unit->GetName());
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("SwarmSpawner: 풀에 없는 유닛 '%s'을(를) 반환하려 시도했습니다."), *Unit->GetName());
}

void ASLSwarmSpawner::ExpandPool(const TSubclassOf<ACharacter>& UnitClass, const int32 Count)
{
	UWorld* World = GetWorld();
	if (!World || !UnitClass || Count <= 0) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;

	for (int32 i = 0; i < Count; i++)
	{
		if (ObjectPool.Num() >= MaxPoolSize)
		{
			UE_LOG(LogTemp, Warning, TEXT("SwarmSpawner: 최대 풀 크기(%d)에 도달하여 더 이상 풀을 확장할 수 없습니다."), MaxPoolSize);
			break;
		}

		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, -10000);
		ACharacter* NewUnit = World->SpawnActor<ACharacter>(
			UnitClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (NewUnit)
		{
			FPooledUnit NewEntry;
			NewEntry.Character = NewUnit;
			NewEntry.bInUse = false;
			NewEntry.UnitClass = UnitClass;

			if (ASLMonsterAICharacter* Monster = Cast<ASLMonsterAICharacter>(NewUnit))
			{
				Monster->ToggleWeaponState(false);
			}

			NewUnit->SetActorHiddenInGame(true);
			NewUnit->SetActorEnableCollision(false);
			NewUnit->SetActorTickEnabled(false);
			NewUnit->OnDestroyed.AddDynamic(this, &ASLSwarmSpawner::OnUnitDestroyed);

			ObjectPool.Add(NewEntry);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SwarmSpawner: 유닛 스폰 실패: %s"), *UnitClass->GetName());
		}
	}
}

void ASLSwarmSpawner::CleanupPool()
{
	TArray<FPooledUnit> PoolCopy = ObjectPool;

	for (FPooledUnit& PoolEntry : PoolCopy)
	{
		if (IsValid(PoolEntry.Character) && !PoolEntry.Character->IsActorBeingDestroyed())
		{
			OnUnitActuallyDestroyed.Broadcast(PoolEntry.Character);
			PoolEntry.Character->Destroy();
		}
	}
	ObjectPool.Empty();
	UE_LOG(LogTemp, Log, TEXT("SwarmSpawner: 풀 정리 완료."));
}

ACharacter* ASLSwarmSpawner::GetOrCreateAndPlaceUnit(TSubclassOf<ACharacter> UnitClass)
{
	if (!UnitClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SwarmSpawner: GetOrCreateAndPlaceUnit에 유효하지 않은 UnitClass가 전달되었습니다."));
		return nullptr;
	}

	const ACharacter* DefaultCharacter = UnitClass->GetDefaultObject<ACharacter>();
	const float CapsuleHalfHeight = DefaultCharacter ? DefaultCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 50.0f;

	ACharacter* SpawnedUnit = GetPooledUnit(UnitClass);

	if (!SpawnedUnit)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwarmSpawner: 풀에서 유닛을 가져오지 못하여 새로 스폰 시도: %s"), *UnitClass->GetName());
		UWorld* World = GetWorld();
		if (!World) return nullptr;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.bNoFail = true;

		FVector SpawnLocation = GetRandomSpawnLocation();
		SpawnLocation.Z += CapsuleHalfHeight;
		const FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

		SpawnedUnit = World->SpawnActor<ACharacter>(
			UnitClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);

		if (!SpawnedUnit)
		{
			UE_LOG(LogTemp, Error, TEXT("SwarmSpawner: 풀 대체 스폰 실패: %s"), *UnitClass->GetName());
			return nullptr;
		}
	}
	else
	{
		FVector SpawnLocation = GetRandomSpawnLocation();
		SpawnLocation.Z += CapsuleHalfHeight;
		const FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

		SpawnedUnit->SetActorLocation(
			SpawnLocation,
			false,
			nullptr,
			ETeleportType::TeleportPhysics
		);
		SpawnedUnit->SetActorRotation(SpawnRotation);
	}

	// 무기 보임
	if (ASLMonsterAICharacter* MonsterAI = Cast<ASLMonsterAICharacter>(SpawnedUnit))
	{
		MonsterAI->ToggleWeaponState(true);
	}

	return SpawnedUnit;
}

void ASLSwarmSpawner::ConfigureSpawnedUnitInternal(ACharacter* SpawnedUnit, TSubclassOf<AAIController> ControllerClass,
                                                   FGenericTeamId TeamID, float AvoidanceWeight)
{
	if (!IsValid(SpawnedUnit)) return;

	if (ASLMonsterAICharacter* MonsterAI = Cast<ASLMonsterAICharacter>(SpawnedUnit))
	{
		if (MonsterAI->IsInPrimaryState(TAG_AI_Dead))
		{
			MonsterAI->SetPrimaryState(TAG_AI_Idle);
			MonsterAI->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MonsterAI->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MonsterAI->ToggleWeaponState(true);
		}
	}

	if (UCharacterMovementComponent* MovementComp = SpawnedUnit->GetCharacterMovement())
	{
		MovementComp->Activate();
		MovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
		MovementComp->GravityScale = 1.f;
		MovementComp->AvoidanceWeight = AvoidanceWeight;
		MovementComp->AvoidanceWeight = AvoidanceWeight;
	}

	AController* CurrentController = SpawnedUnit->GetController();
	AAIController* AIController = Cast<AAIController>(CurrentController);
	ASLMonsterAICharacterBase* MonsterAI = Cast<ASLMonsterAICharacterBase>(SpawnedUnit);

	if (!AIController
		|| !AIController->IsA(ControllerClass)
		|| !AIController->GetPawn()
		|| AIController->GetPawn() != SpawnedUnit)
	{
		if (CurrentController)
		{
			if (CurrentController->GetPawn() == SpawnedUnit)
			{
				CurrentController->UnPossess();
			}
			CurrentController->Destroy();
		}

		SpawnedUnit->AIControllerClass = ControllerClass;
		SpawnedUnit->SpawnDefaultController();
		AIController = Cast<AAIController>(SpawnedUnit->GetController());
	}

	if (AIController && (!AIController->GetPawn() || AIController->GetPawn() != SpawnedUnit))
	{
		AIController->Possess(SpawnedUnit);
	}

	if (AIController)
	{
		if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(AIController))
		{
			TeamAgent->SetGenericTeamId(TeamID);
		}
	}

	if (MonsterAI)
	{
		MonsterAI->BornSpawner = this;
	}

	if (USLAIStateComponent* StateComp = SpawnedUnit->FindComponentByClass<USLAIStateComponent>())
	{
		StateComp->Initialize();
	}

	if (UCharacterMovementComponent* MovementComp = SpawnedUnit->GetCharacterMovement())
	{
		MovementComp->AvoidanceWeight = AvoidanceWeight;
	}

	UE_LOG(LogTemp, Log, TEXT("스폰 및 설정된 유닛: %s, 팀: %d"),
	       *SpawnedUnit->GetName(),
	       TeamID.GetId());
}

ACharacter* ASLSwarmSpawner::SpawnAndConfigureUnit(TSubclassOf<ACharacter> UnitClass,
                                                   const TSubclassOf<AAIController> ControllerClass,
                                                   const FGenericTeamId TeamID,
                                                   const float AvoidanceWeight)
{
	ACharacter* SpawnedUnit = GetOrCreateAndPlaceUnit(UnitClass);

	if (!SpawnedUnit)
	{
		return nullptr;
	}

	ConfigureSpawnedUnitInternal(SpawnedUnit, ControllerClass, TeamID, AvoidanceWeight);

	OnUnitSpawned.Broadcast(SpawnedUnit, this);
	return SpawnedUnit;
}

FVector ASLSwarmSpawner::GetRandomSpawnLocation() const
{
	const FVector Origin = GetActorLocation() + FVector(0, 0, 50);
	const float Radius = SpawnBox->GetScaledBoxExtent().X; // 박스 크기를 기반으로 탐색 반경 설정

	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		FNavLocation RandomLocation;
		if (NavSys->GetRandomReachablePointInRadius(Origin, Radius, RandomLocation))
		{
			return RandomLocation.Location;
		}
	}

	FVector BoxExtent = SpawnBox->GetScaledBoxExtent();
	const float X = FMath::RandRange(-BoxExtent.X, BoxExtent.X);
	const float Y = FMath::RandRange(-BoxExtent.Y, BoxExtent.Y);

	return Origin + FVector(X, Y, 100.f);
}

void ASLSwarmSpawner::OnUnitDestroyed(AActor* DestroyedActor)
{
	if (ACharacter* Unit = Cast<ACharacter>(DestroyedActor))
	{
		int32 RemovedCount = ObjectPool.RemoveAll([Unit](const FPooledUnit& Entry)
		{
			return Entry.Character == Unit;
		});

		if (RemovedCount > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("SwarmSpawner: 풀링된 유닛 '%s'이(가) 외부에서 파괴되어 풀에서 제거됨."),
			       *DestroyedActor->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("SwarmSpawner: 풀링되지 않은 유닛 '%s'이(가) 파괴됨."), *DestroyedActor->GetName());
		}
		OnUnitActuallyDestroyed.Broadcast(Unit);
	}
}

int32 ASLSwarmSpawner::GetPooledUnitCount() const
{
	return ObjectPool.Num();
}

int32 ASLSwarmSpawner::GetActiveUnitCount() const
{
	int32 Count = 0;
	for (const FPooledUnit& Entry : ObjectPool)
	{
		if (Entry.bInUse && IsValid(Entry.Character))
		{
			Count++;
		}
	}
	return Count;
}
