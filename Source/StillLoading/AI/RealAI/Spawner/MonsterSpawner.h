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

UCLASS()
class STILLLOADING_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawner();

	UFUNCTION(BlueprintPure)
	FVector GetRandomSpawnLocation() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<UBoxComponent> SpawnArea;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugSpawnCapsules = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float RiseHeight = 500.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SpawnMonstersByType();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FMonsterSpawnInfo> SpawnInfos;

private:
	UFUNCTION()
	void SpawnFloorEffect(const AMonsterAICharacter* MonsterActor);
	
	UPROPERTY()
	float SpawnSpacing = 200.f;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedMonsters;
};
