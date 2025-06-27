#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPoolManager.generated.h"

USTRUCT(BlueprintType)
struct FObjectPool
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<AActor>> PooledObjects;
};

UCLASS()
class STILLLOADING_API AAIPoolManager : public AActor
{
	GENERATED_BODY()

public:
	AAIPoolManager();

	UFUNCTION(BlueprintPure, Category = "AIPoolManager")
	static AAIPoolManager* Get(const UObject* WorldContextObject);

	AActor* SpawnFromPool(const TSubclassOf<AActor>& ActorClass, const FTransform& SpawnTransform);
    
	void ReturnToPool(AActor* ActorToReturn);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "AIPoolManager")
	TMap<TSubclassOf<AActor>, int32> PoolConfig;

	UPROPERTY()
	TMap<TSubclassOf<AActor>, FObjectPool> Pools;
};
