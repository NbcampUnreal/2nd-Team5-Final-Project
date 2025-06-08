#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

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
	TSubclassOf<AMonsterAICharacter> Monster;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "100"))
	int32 Count = 1;
};

UCLASS()
class STILLLOADING_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<UBoxComponent> SpawnArea;
	
	UFUNCTION(BlueprintPure)
	FVector GetRandomSpawnLocation() const;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebugSpawnCapsules = true;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SpawnAllMonsters();

	UFUNCTION(BlueprintCallable)
	void SpawnMonstersByType();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FMonsterSpawnInfo> SpawnInfos;

private:
	UPROPERTY()
	float SpawnSpacing = 200.f;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedMonsters;
};
