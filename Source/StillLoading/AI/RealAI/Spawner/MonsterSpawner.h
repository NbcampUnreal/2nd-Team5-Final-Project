#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

class UNiagaraSystem;
class UBoxComponent;
struct FMonsterMeshData;
class UMonsterMeshDataAsset;
class AMonsterAICharacter;
enum class EMonsterType : uint8;

USTRUCT(BlueprintType)
struct FMonsterSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Monster;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "100"))
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FMonsterSpawnedInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> SpawnedMonster;
};

UENUM(BlueprintType)
enum class ESpawnType : uint8
{
	ST_None 	UMETA(DisplayName = "None"),
	ST_Leader	UMETA(DisplayName = "With Leader"),
	ST_NoLeader	UMETA(DisplayName = "Without Leader"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnedMonstersUpdated, int32, DecreaseCount);

UCLASS()
class STILLLOADING_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawner();

	UPROPERTY(BlueprintAssignable, Category = "Monster Spawner | Events")
	FOnSpawnedMonstersUpdated OnMonstersUpdated;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	int32 GetSpawnCount() const;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnActorAtLocation(const TSubclassOf<AActor> ActorToSpawn, const FVector& SpawnLocation, FMonsterSpawnedInfo& OutMonsterInfo);

	UFUNCTION(BlueprintCallable)
	void SpawnMonstersByType();

	UFUNCTION(BlueprintCallable)
	void SpawnMonstersWithoutLeader();

	UFUNCTION(BlueprintCallable)
	void MonsterDied(AActor* DiedMonsterRef);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<UBoxComponent> SpawnArea;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugSpawnCapsules = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float RiseHeight = 500.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	FVector GetRandomSpawnLocation() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FMonsterSpawnInfo> SpawnInfos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	ESpawnType SpawnType = ESpawnType::ST_None;

private:
	UFUNCTION()
	void SpawnFloorEffect(const AMonsterAICharacter* MonsterActor);
	
	UPROPERTY()
	float SpawnSpacing = 200.f;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedMonsters;

	UPROPERTY()
	int32 TotalMonsterCount = 0;

	UPROPERTY()
	int32 LastMonsterCount = 0;
};
