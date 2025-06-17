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
	EMouseActorState GetCurrentState() const;

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void SetChaseSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void SetGrabSettings(float NewGrabDistance, float NewGrabHeight, float NewGrabDamage, float NewGrabDuration, float NewGrabCooldown);

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
	void MoveTowardsPlayer(float DeltaTime);
	void MoveTowardsGrabTarget(float DeltaTime);
	void UpdateMeshRotation(float DeltaTime);
	void StartGrabPlayer();
	void CompleteGrabPlayer();
	void ApplyGrabDamage();
	void SetMouseActorState(EMouseActorState NewState);
	ASLPlayerCharacter* FindPlayerCharacter();
	bool IsPlayerInRange() const;
	bool CanGrabPlayer() const;

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
	float GrabDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float GrabHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float GrabDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float GrabDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float GrabCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Settings")
	float DetectionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> GrabEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> GrabSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> DestroySound;

	
private:
	EMouseActorState CurrentState;
	float CurrentHealth;
	TObjectPtr<ASLPlayerCharacter> TargetPlayer;
	FVector GrabStartLocation;
	FVector GrabTargetLocation;
	FTimerHandle GrabTimerHandle;
	FTimerHandle GrabCooldownTimerHandle;
	bool bCanGrab;
	
};