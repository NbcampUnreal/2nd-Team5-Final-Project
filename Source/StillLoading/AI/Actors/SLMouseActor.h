#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SLMouseActor.generated.h"

class UBattleComponent;
class UNiagaraSystem;
class ASLPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnMouseActorDestroyed, ASLMouseActor*, MouseActor);

UENUM(BlueprintType)
enum class EMouseActorState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Chasing		UMETA(DisplayName = "Chasing"),
	Attacking	UMETA(DisplayName = "Attacking"),
	Grabbing	UMETA(DisplayName = "Grabbing"),
	Destroyed	UMETA(DisplayName = "Destroyed")
};

UCLASS()
class STILLLOADING_API ASLMouseActor : public AActor
{
	GENERATED_BODY()

public:
	ASLMouseActor();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void StartChasing();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void StopChasing();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void DestroyMouseActor();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void GrabAndMovePlayer(const FVector& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void SetChaseSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void SetAttackDamage(float NewDamage);

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	EMouseActorState GetCurrentState() const;

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void TakeDamage(float DamageAmount);

	UPROPERTY(BlueprintAssignable, Category = "Mouse Actor")
	FSLOnMouseActorDestroyed OnMouseActorDestroyed;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType);

	void UpdateChaseTarget();
	void ChasePlayer();
	void AttackPlayer();
	void UpdateThreatEffects();
	void PlayDestroyEffects();
	void SetMouseActorState(EMouseActorState NewState);
	void MoveTowardsPlayer(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MouseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBattleComponent> BattleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float ChaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float GrabForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float ThreatEffectInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float AttackCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ChaseEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> AttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ThreatEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> ChaseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> DestroySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TObjectPtr<UMaterialInterface> NormalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TObjectPtr<UMaterialInterface> ThreatMaterial;

private:
	ASLPlayerCharacter* FindPlayerCharacter();
	bool IsPlayerInAttackRange();
	void HandlePlayerGrabbing();
	void ResetAttackCooldown();

	EMouseActorState CurrentState;
	float CurrentHealth;
	TObjectPtr<ASLPlayerCharacter> TargetPlayer;
	FTimerHandle ThreatEffectTimerHandle;
	FTimerHandle AttackCooldownHandle;
	FVector LastPlayerLocation;
	FVector TargetLocation;
	bool bCanAttack;
	bool bIsGrabbing;
};