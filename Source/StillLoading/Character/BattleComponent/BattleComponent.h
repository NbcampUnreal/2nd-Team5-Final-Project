#pragma once

#include "CoreMinimal.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "Components/ActorComponent.h"
#include "BattleComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCharacterHited, AActor*, DamageCauser, float, DamageAmount, const FHitResult&, HitResult, EAttackAnimType, AnimType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UBattleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBattleComponent();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void SendHitResult(AActor* HitTarget, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType);

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void ReceiveHitResult(float DamageAmount, AActor* DamageCauser, const FHitResult& HitResult,
						  EAttackAnimType AnimType);
	UFUNCTION()
	void DoAttackSweep(EAttackAnimType AttackType);
	UFUNCTION()
	void ClearHitTargets();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebugLine = false;
	
	UPROPERTY(BlueprintAssignable, Category = "Delegate | Battle")
	FOnCharacterHited OnCharacterHited;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset")
	TObjectPtr<UAttackDataAsset> AttackData;
private:
	UFUNCTION(BlueprintCallable, Category = "Attack")
	float GetDamageByType(EAttackAnimType InType) const;
	
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AlreadyHitActors;
};
