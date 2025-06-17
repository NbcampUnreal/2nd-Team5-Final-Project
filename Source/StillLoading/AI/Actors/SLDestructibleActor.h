#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDestructibleActor.generated.h"

class UBattleComponent;
class UNiagaraSystem;

enum class EHitAnimType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnDestructibleDestroyed, int32, ActorIndex);

UENUM(BlueprintType)
enum class EDestructibleState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Active		UMETA(DisplayName = "Active"),
	Destroyed	UMETA(DisplayName = "Destroyed")
};

UCLASS(BlueprintType, Abstract)
class STILLLOADING_API ASLDestructibleActor : public AActor
{
	GENERATED_BODY()

public:
	ASLDestructibleActor();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	virtual void ActivateActor();

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	virtual void DeactivateActor();

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	virtual void DestroyActor();

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	bool CanBeDestroyed() const;

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	void SetActorIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	int32 GetActorIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	EDestructibleState GetCurrentState() const;

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Destructible")
	void TakeDamage(float DamageAmount);

	UPROPERTY(BlueprintAssignable, Category = "Destructible")
	FSLOnDestructibleDestroyed OnDestructibleDestroyed;

protected:
	UFUNCTION()
	virtual void OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType);

	virtual void SetupCollisionResponse();
	virtual void UpdateActorVisuals();
	virtual void PlayDestroyEffects();
	virtual void SetActorState(EDestructibleState NewState);
	virtual void OnActorDestroyed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBattleComponent> BattleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible Settings")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> DestroySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> ActivationSound;

private:
	EDestructibleState CurrentState;
	float CurrentHealth;
	int32 ActorIndex;
};