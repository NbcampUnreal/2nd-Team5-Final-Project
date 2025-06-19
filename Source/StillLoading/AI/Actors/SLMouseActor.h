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
	Orbiting	UMETA(DisplayName = "Orbiting"),
	Descending	UMETA(DisplayName = "Descending"),
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
	void StartOrbiting();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void StopOrbiting();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void DestroyMouseActor();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	EMouseActorState GetCurrentState() const;

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void SetOrbitSettings(float NewOrbitRadius, float NewOrbitHeight, float NewOrbitSpeed);

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void SetGrabSettings(float NewGrabDistance, float NewGrabHeight, float NewGrabDamage, float NewGrabCooldownMin, float NewGrabCooldownMax);

	UPROPERTY(BlueprintAssignable, Category = "Mouse Actor")
	FSLOnMouseActorDestroyed OnMouseActorDestroyed;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType);

	void UpdateOrbitMovement(float DeltaTime);
	void UpdateDescentMovement(float DeltaTime);
	void UpdateGrabMovement(float DeltaTime);
	void UpdateMeshRotation(float DeltaTime);
	void StartGrabPlayer();
	void CompleteGrabPlayer();
	void ApplyGrabDamage();
	void SetMouseActorState(EMouseActorState NewState);
	ASLPlayerCharacter* FindPlayerCharacter();
	bool IsPlayerInRange() const;
	float GetRandomGrabCooldown() const;
	bool CanGrabPlayer() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MouseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBattleComponent> BattleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Settings")
	float OrbitRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Settings")
	float OrbitHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Settings")
	float OrbitSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
	float DescentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
	float GrabMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Settings")
	float GrabDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Settings")
	float GrabHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Settings")
	float GrabDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Settings")
	float GrabCooldownMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Settings")
	float GrabCooldownMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Settings")
	float CollisionDisableTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Settings")
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
	FVector GrabTargetLocation;
	FTimerHandle GrabCooldownTimerHandle;
	FTimerHandle CollisionTimerHandle;
	bool bCanGrab;
	float OrbitAngle;
	FVector OrbitCenter;
};