#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SLBattleInterface.generated.h"

UENUM(BlueprintType)
enum class EAttackAnimType : uint8 // 애니메이션 기준
{
	AAT_NormalAttack UMETA(DisplayName = "Normal Attack"),
};

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
	void SendHitResult(AActor* HitTarget, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Battle")
	void ReceiveHitResult(float DamageAmount, AActor* DamageCauser, const FHitResult& HitResult, EAttackAnimType AnimType);
};
