#pragma once

#include "CoreMinimal.h"
#include "Character/Interface/SLBattleInterface.h"
#include "Components/ActorComponent.h"
#include "BattleComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCharacterHited, AActor*, DamageCauser, float, DamageAmount, const FHitResult&, HitResult, EAttackAnimType, AnimType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UBattleComponent : public UActorComponent, public ISLBattleInterface
{
	GENERATED_BODY()

public:
	UBattleComponent();
	
	virtual void SendHitResult_Implementation(AActor* HitTarget, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType) override;
	virtual void ReceiveHitResult_Implementation(float DamageAmount, AActor* DamageCauser, const FHitResult& HitResult, EAttackAnimType AnimType) override;

	UPROPERTY(BlueprintAssignable, Category = "Delegate | Battle")
	FOnCharacterHited OnCharacterHited;
	
protected:
	virtual void BeginPlay() override;
};
