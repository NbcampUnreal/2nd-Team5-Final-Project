#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLBattleInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USLBattleInterface : public UInterface
{
	GENERATED_BODY()
};

class ISLBattleInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Battle")
	void ReceiveBattleDamage(float DamageAmount);
};
