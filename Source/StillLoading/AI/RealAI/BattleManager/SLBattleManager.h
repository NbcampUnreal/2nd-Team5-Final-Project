#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "SLBattleManager.generated.h"

// 전투관리, 스폰된 유닛 관리, 타겟 관리

class USLAITokenSystemComponent;
class ATargetPoint;
class USLWaveSpawnerComponent;
class UBoxComponent;
class ASLSwarmSpawner;
class USLAIStateComponent;

UENUM(BlueprintType)
enum class EUnitActionPermission : uint8
{
	None UMETA(DisplayName = "None"),
	EngageTarget UMETA(DisplayName = "Engage Target"),
	SupportOrIdle UMETA(DisplayName = "Support or Idle")
};

USTRUCT(BlueprintType)
struct FBattleUnitInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY()
	FGenericTeamId TeamId;

	UPROPERTY()
	bool bIsPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EUnitActionPermission CurrentPermission;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> CurrentEngagedTarget;
    
	UPROPERTY()
	TObjectPtr<USLAIStateComponent> WarComponent = nullptr;

	UPROPERTY()
	TObjectPtr<ASLSwarmSpawner> SourceSpawner = nullptr;
};

USTRUCT(BlueprintType)
struct FSpawnerTargetPoints
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetPoints")
	TObjectPtr<ASLSwarmSpawner> Spawner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetPoints")
	TArray<ATargetPoint*> TargetPoints;
};

USTRUCT(BlueprintType)
struct FTeamIndicesArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int32> Indices;
};

UCLASS()
class STILLLOADING_API ASLBattleManager : public AActor
{
	GENERATED_BODY()

public:    
	ASLBattleManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Battle Management")
	void StartBattle();
	UFUNCTION(BlueprintCallable, Category = "Battle Management")
	void EndBattle();
	UFUNCTION(BlueprintCallable, Category = "Battle Management|Waves")
	void StartNextGlobalWave();

	UFUNCTION(BlueprintCallable, Category = "Battle Management")
	TArray<FBattleUnitInfo> GetUnitsSpawnedBySpawner(ASLSwarmSpawner* Spawner) const;
	UFUNCTION(BlueprintCallable)
	void RegisterUnit(AActor* Actor, bool bIsPlayer, ASLSwarmSpawner* SourceSpawner);
	UFUNCTION(BlueprintCallable)
	void UnregisterUnit(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetRegisteredUnitCount() const { return RegisteredUnits.Num(); }

protected:
	void BindToSpawnerEvents(); // ASLSwarmSpawner 이벤트 바인딩
	UFUNCTION()
	void OnSpawnerUnitSpawnedHandler(AActor* SpawnedUnit, ASLSwarmSpawner* SourceSpawner);
	UFUNCTION()
	void OnSpawnerUnitReturnedToPoolHandler(AActor* ReturnedUnit, ASLSwarmSpawner* SourceSpawner);
	UFUNCTION()
	void OnSpawnerUnitActuallyDestroyedHandler(AActor* DestroyedActor);

	// --- 웨이브 컴포넌트 델리게이트 핸들러들 ---
	UFUNCTION()
	void HandleWaveCompleted(int32 WaveNumber, ASLSwarmSpawner* CompletedSpawner);
	UFUNCTION()
	void HandleAllWavesCompleted(ASLSwarmSpawner* CompletedSpawner);
	void RebuildTeamIndices();

	// --- 스포너 관리 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Management|Spawners")
	TArray<ASLSwarmSpawner*> ManagedSpawners;

	// --- 유닛 관리 ---
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TArray<FBattleUnitInfo> RegisteredUnits;
	UPROPERTY(VisibleAnywhere, Category = "Battle Management|Units")
	TMap<FGenericTeamId, FTeamIndicesArrayWrapper> TeamUnitIndices;

	// --- 타겟 포인트 관리 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Management|TargetPoints")
	TArray<FSpawnerTargetPoints> SpawnerTargetPointData;

	// --- 웨이브 관리 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Management|Waves")
	TArray<FSpawnerTargetPoints> CurrentWaveTargetPoints;
	int32 CurrentGlobalWaveNumber;

	UPROPERTY()
	TMap<ASLSwarmSpawner*, bool> SpawnerWaveCompletionStatus;
	
public:
	// 외부에서 호출될 수 있는 유틸리티 함수들
	UFUNCTION(BlueprintPure, Category = "Battle Management|Teams")
	bool AreEnemies(const FGenericTeamId& me, const FGenericTeamId& target, const bool bIsPlayer) const;
	UFUNCTION(BlueprintPure, Category = "Battle Management|Units")
	TArray<FBattleUnitInfo> GetUnitsOfTeam(const FGenericTeamId& TeamId);
	UFUNCTION(BlueprintPure, Category = "Battle Management|Units")
	TArray<FBattleUnitInfo> GetEnemiesOfTeam(const FGenericTeamId& TeamId);
	
	// AI 워 컴포넌트가 다음 타겟 포인트를 요청할 때 사용
	UFUNCTION(BlueprintCallable, Category = "Battle Management|TargetPoints")
	FVector GetNextTargetPointLocationForSpawner(ASLSwarmSpawner* ForSpawner, int32& CurrentTargetIndex) const;

	// 전투 Permission 관리
	UFUNCTION(BlueprintCallable, Category = "Battle Management|Permissions")
	EUnitActionPermission GetUnitActionPermission(AActor* Unit) const;
	UFUNCTION(BlueprintCallable, Category = "Battle Management|Permissions")
	bool RequestEngagementPermission(AActor* RequestingUnit, AActor* TargetActor);
	
private:
	void ReassignPermissions();
	FTimerHandle PermissionReassignmentTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Battle Management|Permissions")
	float PermissionReassignmentInterval = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Battle Management|Permissions")
	int32 MaxEngagingUnitsPerTarget = 5;
};