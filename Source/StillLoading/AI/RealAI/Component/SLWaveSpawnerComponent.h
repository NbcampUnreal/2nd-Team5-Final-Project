#pragma once

#include "CoreMinimal.h"
#include "AI/RealAI/BattleManager/SLBattleTypes.h"
#include "AI/RealAI/Spawner/SLSwarmSpawner.h"
#include "Components/ActorComponent.h"
#include "SLWaveSpawnerComponent.generated.h"

class ASLSwarmSpawner;
class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveCompleted, int32, WaveNumber, USLWaveSpawnerComponent*, CompletedSpawner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllWavesCompleted, USLWaveSpawnerComponent*, CompletedSpawner);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLWaveSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLWaveSpawnerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 에디터에서 설정할 웨이브 데이터 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Spawning|Config")
	TArray<FWaveData> Waves;

	UFUNCTION(BlueprintCallable, Category = "Wave Spawner")
	TArray<FSimpleSpawnComposition> GetAllWaveCompositions() const;

	// ASwarmSpawner가 이 컴포넌트에게 특정 웨이브를 시작하도록 지시할 함수
	UFUNCTION(BlueprintCallable, Category = "Wave Spawning")
	bool StartWaveByIndex(int32 WaveIndex); // 특정 웨이브 인덱스를 받아 시작

	// 현재 스폰 중인 웨이브를 중지
	UFUNCTION(BlueprintCallable, Category = "Wave Spawning")
	void StopWaveSpawning();

	// 현재 웨이브 스폰이 활성화되어 있는지 확인
	UFUNCTION(BlueprintPure, Category = "Wave Spawning")
	bool IsWaveSpawningActive() const;

	// 무한 스폰
	UFUNCTION(BlueprintCallable)
	bool IsInfiniteRespawnMode() const { return bEnableInfiniteRespawn; }
	UFUNCTION()
	void OnUnitReturnedToPool(TSubclassOf<ACharacter> UnitClass);
	UFUNCTION()
	void ImmediateRespawnFromPool(TSubclassOf<ACharacter> UnitClass);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infinite Respawn")
	bool bEnableInfiniteRespawn = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infinite Respawn", meta = (EditCondition = "bEnableInfiniteRespawn"))
	float RespawnDelay = 5.0f;

	// 웨이브 완료 시 호출될 델리게이트 (ASwarmSpawner가 리슨)
	UPROPERTY(BlueprintAssignable, Category = "Wave Spawning|Events")
	FOnWaveCompleted OnWaveCompleted;
	UPROPERTY(BlueprintAssignable, Category = "Wave Spawning|Events")
	FOnAllWavesCompleted OnAllWavesCompleted;

protected:
	// 내부 스폰 로직 (웨이브 데이터 기반)
	void StartCurrentWaveSpawning();
	void FinishCurrentWave();

	UPROPERTY(Transient)
	TObjectPtr<ASLSwarmSpawner> CachedOwnerSpawner;

	int32 CurrentWaveIndex; // 현재 진행 중인 웨이브의 인덱스
	bool bIsSpawningActive; // 스폰 활성화 상태

	FTimerHandle WaveSpawnTimerHandle; // 웨이브 스폰 타이머 핸들

private:
	// 무한 스폰용
	UFUNCTION()
	void InitializeInfiniteRespawnMode();
	UFUNCTION()
	void CheckAndRespawnUnits();
	UFUNCTION()
	void RespawnUnit(TSubclassOf<ACharacter> UnitClass, const FWaveCompositionData& CompositionData);
	
	UPROPERTY()
	TMap<TSubclassOf<ACharacter>, int32> TargetUnitCounts; // 각 유닛 타입별 목표 개수
	UPROPERTY()
	TMap<TSubclassOf<ACharacter>, int32> CurrentUnitCounts; // 현재 살아있는 개수
	UPROPERTY()
	TMap<TSubclassOf<ACharacter>, FWaveCompositionData> UnitCompositionData; // 리스폰용 데이터
	UPROPERTY()
	FTimerHandle RespawnCheckTimerHandle;
};
