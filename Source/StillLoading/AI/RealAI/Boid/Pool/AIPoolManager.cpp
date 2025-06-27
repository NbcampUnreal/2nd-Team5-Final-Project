#include "AIPoolManager.h"

#include "Kismet/GameplayStatics.h"

AAIPoolManager::AAIPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

AAIPoolManager* AAIPoolManager::Get(const UObject* WorldContextObject)
{
	return Cast<AAIPoolManager>(UGameplayStatics::GetActorOfClass(WorldContextObject, AAIPoolManager::StaticClass()));
}

void AAIPoolManager::BeginPlay()
{
	Super::BeginPlay();

	for (const auto& Elem : PoolConfig)
	{
		TSubclassOf<AActor> ActorClass = Elem.Key;
		const int32 PoolSize = Elem.Value;

		if (ActorClass)
		{
			FObjectPool NewPool;
			for (int32 i = 0; i < PoolSize; ++i)
			{
				if (AActor* PooledObject = GetWorld()->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator))
				{
					PooledObject->SetActorHiddenInGame(true);
					PooledObject->SetActorEnableCollision(false);
					PooledObject->SetActorTickEnabled(false);
					NewPool.PooledObjects.Add(PooledObject);
				}
			}
			Pools.Add(ActorClass, NewPool);
		}
	}
}

AActor* AAIPoolManager::SpawnFromPool(const TSubclassOf<AActor>& ActorClass, const FTransform& SpawnTransform)
{
	if (!Pools.Contains(ActorClass)) return nullptr;

	FObjectPool& Pool = Pools[ActorClass];
	for (AActor* PooledActor : Pool.PooledObjects)
	{
		if (PooledActor && !PooledActor->IsActorTickEnabled())
		{
			PooledActor->SetActorTransform(SpawnTransform);
			PooledActor->SetActorHiddenInGame(false);
			PooledActor->SetActorEnableCollision(true);
			PooledActor->SetActorTickEnabled(true);
            
			// TODO: 재사용을 위한 AI 초기화 함수 호출 (예: AI->Activate())
            
			return PooledActor;
		}
	}
    
	// TODO: 만약 풀에 남는 액터가 없다면 새로 생성하거나 null 반환 (풀 크기 동적 확장)
	return nullptr;
}

void AAIPoolManager::ReturnToPool(AActor* ActorToReturn)
{
	if (ActorToReturn)
	{
		ActorToReturn->SetActorHiddenInGame(true);
		ActorToReturn->SetActorEnableCollision(false);
		ActorToReturn->SetActorTickEnabled(false);
	}
}




