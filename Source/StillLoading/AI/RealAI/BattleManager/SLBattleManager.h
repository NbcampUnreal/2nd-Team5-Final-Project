#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "SLBattleManager.generated.h"

class USLAITokenSystemComponent;
class ATargetPoint;
class USLWaveSpawnerComponent;
class UBoxComponent;
class ASLSwarmSpawner;
class USLAIStateComponent;

USTRUCT(BlueprintType)
struct FBattleUnitInfo
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Actor = nullptr;

	UPROPERTY()
	FGenericTeamId TeamId;

	UPROPERTY()
	bool bIsPlayer = false;
    
	UPROPERTY()
	USLAIStateComponent* WarComponent = nullptr;

	UPROPERTY()
	USLAITokenSystemComponent* TokenComponent = nullptr;

	UPROPERTY()
	ASLSwarmSpawner* SourceSpawner = nullptr;
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

UENUM(BlueprintType)
enum class ETokenType : uint8
{
	None UMETA(DisplayName = "None"),
	Movement UMETA(DisplayName = "Movement"),
	Attack UMETA(DisplayName = "Attack"),        // 공격
	Skill UMETA(DisplayName = "Skill"),          // 스킬 사용
};

USTRUCT()
struct FTokenRequest
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USLAITokenSystemComponent> Requester;

	UPROPERTY()
	ETokenType TokenType;

	UPROPERTY()
	float RequestTime;

	UPROPERTY()
	int32 Priority;
};

USTRUCT()
struct FTokenRequestQueueWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FTokenRequest> Requests;
};

UCLASS()
class STILLLOADING_API ASLBattleManager : public AActor
{
	GENERATED_BODY()

public:    
    ASLBattleManager();

protected:
    virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
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

	// 토큰 요청 (USLAITokenSystemComponent에서 호출)
	UFUNCTION(BlueprintCallable, Category = "AI Token System")
	bool RequestToken(USLAITokenSystemComponent* Requester, ETokenType TokenType, int32 Priority = 0);

	// 토큰 반납 (USLAITokenSystemComponent에서 호출)
	UFUNCTION(BlueprintCallable, Category = "AI Token System")
	void ReturnToken(USLAITokenSystemComponent* Requester, ETokenType TokenType);

private:
	// --- 토큰 관리 ---
	void ProcessTokenRequests(); // 대기열 처리용
	
	UPROPERTY(EditAnywhere, Category = "AI Token System")
	TMap<ETokenType, int32> MaxTokensPerType;

	UPROPERTY()
	TMap<ETokenType, int32> CurrentAvailableTokens;

	UPROPERTY()
	TMap<ETokenType, FTokenRequestQueueWrapper> TokenRequestQueues;
};
