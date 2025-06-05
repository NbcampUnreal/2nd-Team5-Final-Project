#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyDeathReceiver.generated.h"

UINTERFACE(BlueprintType)
class UEnemyDeathReceiver : public UInterface
{
	GENERATED_BODY()
};

class IEnemyDeathReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void OnEnemyDeath(AActor* DeadAI);
};
