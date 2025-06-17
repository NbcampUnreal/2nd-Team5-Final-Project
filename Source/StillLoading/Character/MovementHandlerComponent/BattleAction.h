#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BattleAction.generated.h"

UINTERFACE(MinimalAPI)
class UBattleAction : public UInterface
{
	GENERATED_BODY()
};

class STILLLOADING_API IBattleAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Battle")
	void Move();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Battle")
	void Attack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Battle")
	void TakeDamage(float Damage);
};