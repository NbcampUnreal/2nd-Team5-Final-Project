#include "SLSwarmSpawner.h"

#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "AI/RealAI/SLMonsterAICharacterBase.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "AI/RealAI/Component/SLAIAttributeComponent.h"
#include "AI/RealAI/Component/SLAIStateComponent.h"
#include "AI/RealAI/Component/SLWaveSpawnerComponent.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
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

void ASLSwarmSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
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
	if (SpawnQueue.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(PoolExpansionTimer);
		UE_LOG(LogTemp, Log, TEXT("SwarmSpawner: 객체 풀 분산 스폰 완료. 총 %d 유닛 풀링됨."), ObjectPool.Num());
		return;
	}
	FQueuedSpawnRequest* CurrentRequest = SpawnQueue.Peek();

	constexpr int32 MaxSpawnThisFrame = 5;
	const int32 NumToSpawn = FMath::Min(CurrentRequest->Count, MaxSpawnThisFrame);

	ExpandPool(CurrentRequest->ClassToSpawn, NumToSpawn);

	CurrentRequest->Count -= NumToSpawn;
	if (CurrentRequest->Count <= 0)
	{
		SpawnQueue.Dequeue(*CurrentRequest);
	}
}

void ASLSwarmSpawner::InitializeObjectPool(const TArray<FSimpleSpawnComposition>& AllCompositionsToPool)
{
	ObjectPool.Empty();
	SpawnQueue.Empty();

	TMap<TSubclassOf<ACharacter>, int32> RequiredUnitsPerClass;
	for (const FSimpleSpawnComposition& Composition : AllCompositionsToPool)
	{
		if (Composition.UnitClass)
		{
			RequiredUnitsPerClass.FindOrAdd(Composition.UnitClass) += Composition.SpawnCount;
		}
	}

	int32 TotalRequired = 0;
	for (auto const& Elem : RequiredUnitsPerClass)
	{
		TotalRequired += Elem.Value;
	}
	if (TotalRequired < InitialPoolSize && RequiredUnitsPerClass.Num() > 0)
	{
		TSubclassOf<ACharacter> DefaultUnitClass = RequiredUnitsPerClass.CreateConstIterator()->Key;
		RequiredUnitsPerClass.FindOrAdd(DefaultUnitClass) += (InitialPoolSize - TotalRequired);
	}

	for (auto const& Elem : RequiredUnitsPerClass)
	{
		if (Elem.Value > 0)
		{
			SpawnQueue.Enqueue({Elem.Key, Elem.Value});
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

			if (AController* Controller = PoolEntry.Character->GetController())
			{
				Controller->SetActorTickEnabled(true);
				if (AAIController* AIController = Cast<AAIController>(Controller))
				{
					AIController->StopMovement();
				}
			}
			return PoolEntry.Character;
		}
	}

	if (bExpandPoolIfNeeded && ObjectPool.Num() < MaxPoolSize)
	{
		UE_LOG(LogTemp, Log, TEXT("SwarmSpawner: 풀 확장 (유닛 클래스: %s, 5개)."), *UnitClass->GetName());
		ExpandPool(UnitClass, 5);
		return GetPooledUnit(UnitClass);
	}

	UE_LOG(LogTemp, Warning, TEXT("SwarmSpawner: 풀에 사용 가능한 유닛이 없거나 최대 풀 크기에 도달함 (%s)."), *UnitClass->GetName());
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

void ASLSwarmSpawner::ExpandPool(TSubclassOf<ACharacter> UnitClass, int32 Count)
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

			if (USkeletalMeshComponent* Mesh = NewUnit->GetMesh())
			{
				Mesh->SetSimulatePhysics(false);
			}

			if (UCharacterMovementComponent* MoveComp = NewUnit->GetCharacterMovement())
			{
				MoveComp->SetMovementMode(EMovementMode::MOVE_None);
				MoveComp->Deactivate();
			}

			if (ASLMonsterAICharacter* Monster = Cast<ASLMonsterAICharacter>(NewUnit))
			{
				Monster->ToggleWeaponState(false);
			}

			// 초기 상태로 비활성화
			NewUnit->SetActorHiddenInGame(true);
			NewUnit->SetActorEnableCollision(false);
			NewUnit->SetActorTickEnabled(false);

			// 파괴 이벤트 바인딩 (풀링된 유닛이 외부에서 파괴될 경우를 대비)
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

	ACharacter* SpawnedUnit = GetPooledUnit(UnitClass);

	if (!SpawnedUnit)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwarmSpawner: 풀에서 유닛을 가져오지 못하여 새로 스폰 시도: %s"), *UnitClass->GetName());
		UWorld* World = GetWorld();
		if (!World) return nullptr;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.bNoFail = true;

		const FVector SpawnLocation = GetRandomSpawnLocation();
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
		const FVector SpawnLocation = GetRandomSpawnLocation();
		const FRotator SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

		SpawnedUnit->SetActorLocation(SpawnLocation);
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
	}

	AController* CurrentController = SpawnedUnit->GetController();
	AAIController* AIController = Cast<AAIController>(CurrentController);
	ASLMonsterAICharacterBase* MonsterAI = Cast<ASLMonsterAICharacterBase>(SpawnedUnit);

	if (!AIController || !AIController->IsA(ControllerClass) || !AIController->GetPawn() || AIController->GetPawn() !=
		SpawnedUnit)
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
		MonsterAI->BattleManager = CachedBattleManager.Get();
		MonsterAI->AIAttributeComp->SetAIStat(EAIChapterType::Chapter4, EAIUnitType::Normal);
	}

	if (USLAIStateComponent* WarComp = SpawnedUnit->FindComponentByClass<USLAIStateComponent>())
	{
		WarComp->Initialize();
	}

	if (UCharacterMovementComponent* MovementComp = SpawnedUnit->GetCharacterMovement())
	{
		MovementComp->AvoidanceWeight = AvoidanceWeight;
	}

	UE_LOG(LogTemp, Log, TEXT("스폰 및 설정된 유닛: %s, 팀: %d"),
	       *SpawnedUnit->GetName(),
	       TeamID.GetId());

	if (SpawnEffectTemplate)
	{
		const FVector EffectLocation = SpawnedUnit->GetActorLocation() + FVector(0.f, 0.f, EffectSpawnHeightOffset);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			SpawnEffectTemplate,
			EffectLocation,
			FRotator::ZeroRotator
		);
	}
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
	const FVector BoxExtent = SpawnBox->GetScaledBoxExtent();
	const FVector Origin = GetActorLocation();

	const float X = FMath::RandRange(-BoxExtent.X, BoxExtent.X);
	const float Y = FMath::RandRange(-BoxExtent.Y, BoxExtent.Y);

	return Origin + FVector(X, Y, 0.f);
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
