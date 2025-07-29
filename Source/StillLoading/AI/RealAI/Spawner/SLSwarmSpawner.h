#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "SLSwarmSpawner.generated.h"

// 풀링 기반 AI 스폰

class USLWaveSpawnerComponent;
class ASLBattleManager;
class UNiagaraSystem;
class UBoxComponent;
class AAIController;

USTRUCT(BlueprintType)
struct FWaveCompositionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<class ACharacter> UnitClass; // 스폰할 유닛 클래스

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<class AAIController> ControllerClass; // 유닛에 할당할 AI 컨트롤러 클래스

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 SpawnCount = 1; // 스폰할 개수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	FGenericTeamId TeamID; // 유닛 팀 ID

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float AvoidanceWeight = 0.5f; // 유닛 Avoidance Weight
};

USTRUCT()
struct FPooledUnit
{
	GENERATED_BODY()

	UPROPERTY()
	ACharacter* Character = nullptr;

	UPROPERTY()
	bool bInUse = false;

	UPROPERTY()
	TSubclassOf<ACharacter> UnitClass;
};

// 단순 스폰 구성 (기존 그대로 유지)
USTRUCT(BlueprintType)
struct FSimpleSpawnComposition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ACharacter> UnitClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 SpawnCount = 1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveCompletedBySpawner, int32, WaveNumber, ASLSwarmSpawner*, CompletedSpawner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllWavesCompletedBySpawner, ASLSwarmSpawner*, CompletedSpawner);

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
    virtual void OnConstruction(const FTransform& Transform) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void ReturnAllActiveUnitsToPool();
	
    // ASLBattleManager가 이 스포너에게 웨이브 시작을 지시할 함수
    UFUNCTION(BlueprintCallable, Category = "Spawner|WaveControl")
    bool StartWave(int32 WaveIndex); // 특정 웨이브 인덱스 시작

    // ASLBattleManager가 이 스포너에게 웨이브 스폰 중지를 지시할 함수
    UFUNCTION(BlueprintCallable, Category = "Spawner|WaveControl")
    void StopWaveSpawning();

    // ASLBattleManager에게 웨이브 완료를 알릴 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Spawner|Events")
    FOnWaveCompletedBySpawner OnWaveCompletedBySpawner;
    UPROPERTY(BlueprintAssignable, Category = "Spawner|Events")
    FOnAllWavesCompletedBySpawner OnAllWavesCompletedBySpawner;

    // ASwarmSpawner의 유닛 스폰/반환/파괴 이벤트 (ASLBattleManager가 리슨)
    UPROPERTY(BlueprintAssignable, Category = "Spawner|Events")
    FOnUnitSpawned OnUnitSpawned;
    UPROPERTY(BlueprintAssignable, Category = "Spawner|Events")
    FOnUnitReturnedToPool OnUnitReturnedToPool;
    UPROPERTY(BlueprintAssignable, Category = "Spawner|Events")
    FOnUnitActuallyDestroyed OnUnitActuallyDestroyed;

    // 오브젝트 풀링 관련 UPROPERTY
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectPool")
    int32 InitialPoolSize = 50;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectPool")
    int32 MaxPoolSize = 100;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ObjectPool")
    bool bExpandPoolIfNeeded = true;

    // 스폰 이펙트 UPROPERTY
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnEffect")
	TObjectPtr<UNiagaraSystem> SpawnEffectTemplate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnEffect")
    float EffectSpawnHeightOffset = 50.0f;

    UPROPERTY()
    TArray<FPooledUnit> ObjectPool;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> SpawnBox;

	TWeakObjectPtr<ASLBattleManager> CachedBattleManager;

	// USLWaveSpawnerComponent를 정확한 타입으로 선언하고 생성합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave Spawning")
	TObjectPtr<USLWaveSpawnerComponent> WaveSpawnerComponent; 

    void InitializeObjectPool(const TArray<FSimpleSpawnComposition>& AllCompositionsToPool);
    ACharacter* GetPooledUnit(TSubclassOf<ACharacter> UnitClass);
    void ReturnUnitToPool(ACharacter* Unit);
    void ExpandPool(TSubclassOf<ACharacter> UnitClass, int32 Count);
    void CleanupPool();

    ACharacter* GetOrCreateAndPlaceUnit(TSubclassOf<ACharacter> UnitClass);
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
    UFUNCTION()
    void HandleInternalWaveCompleted(int32 WaveNumber, USLWaveSpawnerComponent* CompletedComp);
    UFUNCTION()
    void HandleInternalAllWavesCompleted(USLWaveSpawnerComponent* CompletedComp);
};
