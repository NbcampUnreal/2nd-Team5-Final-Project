#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "SLBattleManagerInterface.h"
#include "SLBattleTypes.h"
#include "GameFramework/Actor.h"
#include "SLBattleManager.generated.h"

// 전투관리, 스폰된 유닛 관리, 타겟 관리
class USLAICombatComponent;
class USLAILODComponent;
class USLAITokenSystemComponent;
class ATargetPoint;
class USLWaveSpawnerComponent;
class UBoxComponent;
class ASLSwarmSpawner;
class USLAIStateComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitUnregistered, AActor*, UnregisteredUnit);

UCLASS()
class STILLLOADING_API ASLBattleManager : public AActor, public ISLBattleManagerInterface
{
	GENERATED_BODY()

public:    
	ASLBattleManager();

	virtual void StartWave_Implementation(const TArray<ASLSwarmSpawner*>& SpawnersToActivate, int32 WaveIndex = 0) override;
	virtual void StartInfiniteSpawnMode_Implementation(const TArray<ASLSwarmSpawner*>& SpawnersToActivate) override;
	virtual void EndBattle_Implementation(bool bPlayerWon) override;
	virtual int32 GetTotalSpawnedUnitCount_Implementation() const override;
	virtual void ResetSpawnedUnitCount_Implementation() override;
	virtual bool IsBattleInProgress_Implementation() const override;
	virtual void StopBattle_Implementation() override;

	UPROPERTY(BlueprintAssignable, Category = "Battle Management|Events")
	FOnUnitUnregistered OnUnitUnregistered;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void UpdateAllUnitLocations();

public:
	int32 FindNearestEnemy(int32 MyIndex, float InRange) const;

	void RegisterPlayerUnit(const AController* PlayerController);
	void RequestNextPatrolPointForUnit(AActor* Unit);
	
	UFUNCTION(BlueprintCallable, Category = "Battle Management|Waves")
	void StartNextGlobalWave();

	UFUNCTION(BlueprintCallable, Category = "Battle Management")
	TArray<FBattleUnitInfo> GetUnitsSpawnedBySpawner(ASLSwarmSpawner* Spawner) const;
	UFUNCTION(BlueprintCallable)
	void RegisterUnit(AActor* Actor, bool bIsPlayer, ASLSwarmSpawner* SourceSpawner);
	UFUNCTION(BlueprintCallable)
	void UnregisterUnit(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	bool PlayerOnly() const { return bIsPlayerOnly; }
	UFUNCTION(BlueprintCallable, Category = "Battle Management|Units")
	FORCEINLINE int32 GetRegisteredUnitCount() const { return UnitActors.Num(); }

	const TMap<TObjectPtr<AActor>, int32>& GetUnitIndexMap() const { return UnitIndexMap; }
	const TArray<TObjectPtr<AActor>>& GetUnitActors() const { return UnitActors; }
	const TArray<FVector>& GetUnitLocations() const { return UnitLocations; }
	const TArray<FVector>& GetUnitTargetLocations() const { return UnitTargetLocations; }
	
protected:
	void BindToSpawnerEvents(); // ASLSwarmSpawner 이벤트 바인딩
	void InitializeAISupportingMode();
	UFUNCTION()
	void OnSpawnerUnitSpawnedHandler(AActor* SpawnedUnit, ASLSwarmSpawner* SourceSpawner);
	UFUNCTION()
	void OnSpawnerUnitReturnedToPoolHandler(AActor* ReturnedUnit, ASLSwarmSpawner* SourceSpawner);
	UFUNCTION()
	void OnSpawnerUnitActuallyDestroyedHandler(AActor* DestroyedActor);

	// LOD 관리
	void UpdateAILODs();

	// 다대일 전투용
	void UpdateEncounterPositions();
	TArray<FVector> CalculateCirclePositions(const FVector& Center, float Radius, int32 NumSlots) const;

	// --- 스포너 관리 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Management|Spawners")
	TArray<ASLSwarmSpawner*> ManagedSpawners;

	// --- 유닛 관리 DOD ---
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<FVector> UnitLocations;
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<FGenericTeamId> UnitTeamIDs;
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<TObjectPtr<ASLSwarmSpawner>> UnitSourceSpawners;
	UPROPERTY()
	TMap<TObjectPtr<AActor>, int32> UnitIndexMap;
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<TObjectPtr<AActor>> UnitActors;
	int32 PlayerUnitIndex = INDEX_NONE;;

	// LOD 관련 데이터
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<TObjectPtr<USLAILODComponent>> UnitLODComponents;

	// Combat 관련 데이터
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<TObjectPtr<USLAICombatComponent>> UnitCombatComponents;

	// 다대일 전투를 위한 타겟 위치정보 배열
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<FVector> UnitTargetLocations;
	TMap<int32, int32> OccupiedAttackSlots;
	TMap<int32, int32> OccupiedHighLODSlots;
	TMap<int32, int32> OccupiedMediumLODSlots;

	// 교전 관련 데이터
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<int32> UnitEngagedTargetIndices;
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TMap<FGenericTeamId, FTeamIndicesArrayWrapper> TeamUnitIndices;

	int32 CurrentGlobalWaveNumber;

	UPROPERTY()
	TMap<ASLSwarmSpawner*, bool> SpawnerWaveCompletionStatus;
	
public:
	// 외부에서 호출될 수 있는 유틸리티 함수들
	UFUNCTION(BlueprintPure, Category = "Battle Management|Teams")
	bool AreEnemies(const FGenericTeamId& me, const FGenericTeamId& target, const bool bIsPlayer) const;
	UFUNCTION(BlueprintPure, Category = "Battle Management|Units")
	TArray<FBattleUnitInfo> GetUnitsOfTeam(const FGenericTeamId& TeamId);

	// 버서크 모드 관련
	UFUNCTION(BlueprintPure, Category = "Battle Management")
	APawn* GetPrimaryTarget() const { return PrimaryTarget.Get(); }
	UFUNCTION(BlueprintPure, Category = "Battle Management")
	bool IsBerserkMode() const { return bIsBerserkMode; }
	UFUNCTION(BlueprintCallable, Category = "Battle Management")
	void SetBerserkMode() { bIsBerserkMode = true; }
	
	// 전투 Permission 관리
	UFUNCTION(BlueprintCallable, Category = "Battle Management|Permissions")
	bool RequestEngagementPermission(AActor* RequestingUnit, AActor* TargetActor);
	// AI가 교전을 중단할 때 호출하는 함수
	UFUNCTION(BlueprintCallable, Category = "Battle Management|Permissions")
	void ReleaseEngagementPermission(AActor* ReleasingUnit, AActor* TargetActor);
	// 유닛이 파괴되거나 비활성화될 때 호출
	UFUNCTION(BlueprintCallable, Category = "Battle Management|Permissions")
	void OnUnitDestroyed(AActor* DestroyedUnit);
	
private:
	// 여유 AI 리빌딩 로직
	UFUNCTION()
	void ProcessSupportingAIReassignment();
	UFUNCTION()
	TArray<int32> FindSupportingAIs();
	UFUNCTION()
	TArray<AActor*> FindTargetsWithOpenSlots();
	UFUNCTION()
	void AssignSupportingAIToTarget(int32 SupportingAIIndex, AActor* Target);
	int32 GetCurrentEngagementCount(AActor* TargetActor) const;

	FBattleUnitInfo GetUnitInfoByIndex(int32 Index) const;

	UPROPERTY()
	TMap<TObjectPtr<AActor>, int32> TargetEngagementCounts;
	UPROPERTY()
	TMap<TObjectPtr<AActor>, FEngagedUnitsWrapper> EngagedUnitsPerTarget;
	UPROPERTY()
	TWeakObjectPtr<APawn> PrimaryTarget;
	
	UPROPERTY(EditAnywhere, Category = "Battle Management|Permissions")
	int32 MaxEngagingUnitsPerTarget = 3;
	UPROPERTY(EditAnywhere, Category = "Battle Management|Permissions")
	bool bIsBerserkMode = false;
	UPROPERTY(EditAnywhere, Category = "Battle Management|Permissions")
	bool bIsPlayerOnly = true;
	UPROPERTY(EditAnywhere, Category = "Battle Management|Permissions")
	bool bUseLODSystem = false;
	UPROPERTY(EditAnywhere, Category = "Battle Management|Permissions")
	FBattleAILODBudget LODBudget;

	UPROPERTY(EditAnywhere, Category = "Battle Management|Encounter")
	float PressurerCircleRadius = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Battle Management|Encounter")
	float MediumCircleRadius = 1000.0f;
	
	UPROPERTY(EditAnywhere, Category = "AI LOD")
	FLODDistanceSettings LODDistances;
	UPROPERTY()
	FTimerHandle SupportReassignmentTimerHandle;

	bool bIsBattleActive;
	int32 TotalSpawnedUnitCount;
};