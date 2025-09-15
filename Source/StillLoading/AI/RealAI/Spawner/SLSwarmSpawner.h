#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/StreamableManager.h"
#include "SLSwarmSpawner.generated.h"

// 풀링 기반 AI 스폰

class ATargetPoint;
class USLWaveSpawnerComponent;
class ASLBattleManager;
class UNiagaraSystem;
class UBoxComponent;
class AAIController;

struct FQueuedSpawnRequest
{
	TSubclassOf<ACharacter> ClassToSpawn;
	int32 Count;
};

UENUM(BlueprintType)
enum class EUnitPoolState : uint8
{
	Ready,
	InUse,
	PendingReturn
};

USTRUCT()
struct FPooledUnit
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ACharacter> Character = nullptr;
	UPROPERTY()
	TSubclassOf<ACharacter> UnitClass;
	UPROPERTY()
	bool bInUse = false;
	UPROPERTY()
	EUnitPoolState State = EUnitPoolState::Ready;
};

struct FAsyncSpawnRequest
{
	TSoftClassPtr<ACharacter> UnitClass;
	int32 Count;
	TSharedPtr<FStreamableHandle> StreamableHandle;
};

USTRUCT(BlueprintType)
struct FSimpleSpawnComposition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ACharacter> UnitClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 SpawnCount = 1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitSpawned, AActor*, SpawnedUnit, ASLSwarmSpawner*, SourceSpawner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitReturnedToPool, AActor*, ReturnedUnit, ASLSwarmSpawner*, SourceSpawner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitActuallyDestroyed, AActor*, DestroyedActor);

UCLASS()
class STILLLOADING_API ASLSwarmSpawner : public AActor
{
	GENERATED_BODY()

public:
	ASLSwarmSpawner();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> SpawnBox;

public:
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void ReturnAllActiveUnitsToPool();
	UFUNCTION(BlueprintCallable)
	FVector GetNextTargetPointLocation(AActor* Unit, int32& CurrentTargetIndex);
	void MarkUnitAsReady(ACharacter* Unit);
	
    // ASLBattleManager가 이 스포너에게 웨이브 시작을 지시할 함수
    UFUNCTION(BlueprintCallable, Category = "Spawner|WaveControl")
    bool StartWave(int32 WaveIndex); // 특정 웨이브 인덱스 시작
    // ASLBattleManager가 이 스포너에게 웨이브 스폰 중지를 지시할 함수
    UFUNCTION(BlueprintCallable, Category = "Spawner|WaveControl")
    void StopWaveSpawning();

    // ASwarmSpawner의 유닛 스폰/반환/파괴 이벤트 (ASLBattleManager가 리슨)
    UPROPERTY(BlueprintAssignable, Category = "Spawner|Events")
    FOnUnitSpawned OnUnitSpawned;
    UPROPERTY(BlueprintAssignable, Category = "Spawner|Events")
    FOnUnitReturnedToPool OnUnitReturnedToPool;
    UPROPERTY(BlueprintAssignable, Category = "Spawner|Events")
    FOnUnitActuallyDestroyed OnUnitActuallyDestroyed;

	// AI가 순회할 타겟 포인트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Patrol", meta = (MakeEditWidget = "true"))
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;

	// USLWaveSpawnerComponent를 정확한 타입으로 선언하고 생성합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave Spawning")
	TObjectPtr<USLWaveSpawnerComponent> WaveSpawnerComponent;

    void InitializeObjectPool(const TArray<FSimpleSpawnComposition>& AllCompositionsToPool);
    ACharacter* GetPooledUnit(TSubclassOf<ACharacter> UnitClass);
    void ReturnUnitToPool(ACharacter* Unit);
    void ExpandPool(const TSubclassOf<ACharacter>& UnitClass, const int32 Count);
    void CleanupPool();

    ACharacter* GetOrCreateAndPlaceUnit(const TSubclassOf<ACharacter>& UnitClass);
    void ConfigureSpawnedUnitInternal(ACharacter* SpawnedUnit, TSubclassOf<AAIController> ControllerClass, FGenericTeamId TeamID, float AvoidanceWeight);
    UFUNCTION(BlueprintCallable, Category = "Spawner|Spawn")
    ACharacter* SpawnAndConfigureUnit(TSubclassOf<ACharacter> UnitClass, TSubclassOf<AAIController> ControllerClass, FGenericTeamId TeamID, float AvoidanceWeight);
    FVector GetRandomSpawnLocation() const;

    UFUNCTION()
    void OnUnitDestroyed(AActor* DestroyedActor);
    UFUNCTION(BlueprintPure, Category = "ObjectPool")
    int32 GetPooledUnitCount() const;
    UFUNCTION(BlueprintPure, Category = "ObjectPool")
    int32 GetActiveUnitCount() const;

    void SetCachedBattleManager(class ASLBattleManager* NewBattleManager);
	
protected:
	// 오브젝트 풀링 관련 UPROPERTY
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectPool")
	int32 InitialPoolSize = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectPool")
	int32 MaxPoolSize = 400;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectPool")
	bool bExpandPoolIfNeeded = true;

private:
	void ExpandPoolIncrementally();
	void OnUnitClassLoaded();
	
	FTimerHandle PoolExpansionTimer;
	TArray<FPooledUnit> ObjectPool;
	
	TQueue<FAsyncSpawnRequest> SpawnRequestQueue;
	TSharedPtr<FAsyncSpawnRequest> CurrentAsyncRequest;
	
	TWeakObjectPtr<ASLBattleManager> CachedBattleManager;
};
