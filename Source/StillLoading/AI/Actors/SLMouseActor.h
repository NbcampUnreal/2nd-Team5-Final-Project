#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SLMouseActor.generated.h"

class UBattleComponent;
class UNiagaraSystem;
class ASLPlayerCharacter;

UENUM(BlueprintType)
enum class EMouseActorState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Orbiting	UMETA(DisplayName = "Orbiting"),
	Descending	UMETA(DisplayName = "Descending"),
	Grabbing	UMETA(DisplayName = "Grabbing"),
	MovingToOrbit	UMETA(DisplayName = "Moving To Orbit"),
	MovingToDrop	UMETA(DisplayName = "Moving To Drop"),  // 새로 추가
	Destroyed	UMETA(DisplayName = "Destroyed")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnMouseActorDestroyed, ASLMouseActor*, MouseActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLOnMouseStateChanged, EMouseActorState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnMouseGrabCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnMouseOrbitCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnMouseCollisionReenabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnMouseGrabCooldownFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnMouseActorStunned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSLOnMouseActorStunRecovered);

UCLASS()
class STILLLOADING_API ASLMouseActor : public AActor
{
	GENERATED_BODY()

public:
	ASLMouseActor();

	// Public Functions
	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void StartOrbiting();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void StopOrbiting();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	void DestroyMouseActor();

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	EMouseActorState GetCurrentState() const;
	
	UFUNCTION(BlueprintCallable, Category = "Phase 3")
	void StartPhase3HorrorMode();

	UFUNCTION(BlueprintCallable, Category = "Phase 3")
	TSubclassOf<ASLMouseActor> GetPhase3MouseActorClass() const { return Phase3MouseActorClass; }

	UFUNCTION(BlueprintCallable, Category = "Phase 3")
	void PerformSweepAttack();

	UFUNCTION(BlueprintCallable, Category = "Phase 5")
	void ApplyWallStun(float StunDuration);

	UFUNCTION(BlueprintCallable, Category = "Phase 5")
	bool IsStunned() const;

	UFUNCTION(BlueprintCallable, Category = "Phase 5")
	void EnableAttackability();

	UFUNCTION(BlueprintCallable, Category = "Phase 5")
	void DisableAttackability();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugAttackability();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	bool IsAttackable() const { return bIsAttackable; }

	UFUNCTION(BlueprintCallable, Category = "Mouse Actor")
	bool IsGrabbingPlayer() const;
	
	void StartMultiHitTimer();
	void OnMultiHitTimerFinished();

	// Public Variables (Delegates)
	UPROPERTY(BlueprintAssignable, Category = "Mouse Actor")
	FSLOnMouseActorDestroyed OnMouseActorDestroyed;

	UPROPERTY(BlueprintAssignable, Category = "Mouse Actor")
	FSLOnMouseStateChanged OnMouseStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mouse Actor")
	FSLOnMouseGrabCompleted OnMouseGrabCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Mouse Actor")
	FSLOnMouseOrbitCompleted OnMouseOrbitCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Mouse Actor")
	FSLOnMouseCollisionReenabled OnMouseCollisionReenabled;

	UPROPERTY(BlueprintAssignable, Category = "Mouse Actor")
	FSLOnMouseGrabCooldownFinished OnMouseGrabCooldownFinished;

	UPROPERTY(BlueprintAssignable, Category = "Phase 5")
	FSLOnMouseActorStunned OnMouseActorStunned;

	UPROPERTY(BlueprintAssignable, Category = "Phase 5")
	FSLOnMouseActorStunRecovered OnMouseActorStunRecovered;

protected:
	// Protected Functions
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBattleComponentHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EHitAnimType HitAnimType);

	UFUNCTION()
	void OnGrabCooldownFinished();

	UFUNCTION()
	void OnCollisionReenabled();

	UFUNCTION()
	void OnStunRecovery();

	UFUNCTION()
	void OnSweepAttackCooldownFinished();

	void UpdateOrbitMovement(float DeltaTime);
	void UpdateDescentMovement(float DeltaTime);
	void UpdateGrabMovement(float DeltaTime);
	void UpdateMeshRotation(float DeltaTime);
	void UpdateMoveToOrbitMovement(float DeltaTime);
	void StartGrabPlayer();
	void CompleteGrabPlayer();
	void ApplyGrabDamage();
	void SetMouseActorState(EMouseActorState NewState);
	ASLPlayerCharacter* FindPlayerCharacter();
	bool IsPlayerInRange() const;
	float GetRandomGrabCooldown() const;
	bool CanGrabPlayer() const;
	FVector CalculateOrbitPosition() const;
	void UpdatePhase3Movement(float DeltaTime);
	bool IsPlayerLookingAtMe() const;
	void RestoreOriginalAppearance();
	void ExecuteSweepAttack();
	FVector FindSafeDropLocation(const FVector& CurrentLocation) const;
	bool IsLocationSafe(const FVector& Location) const;
	void StartMoveToDropLocation();
	void UpdateMoveToDropMovement(float DeltaTime);
	void DropPlayerNaturally();
	
	// Protected Variables (Components)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MouseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBattleComponent> BattleComponent;

	// Protected Variables (Settings)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Settings")
	float OrbitRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Settings")
	float OrbitHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Settings")
	float OrbitSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Settings")
	float MoveToOrbitSpeed;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	float Phase3ChaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	float Phase3StopDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	float PlayerLookCheckAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	TObjectPtr<UStaticMesh> Phase3HorrorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	FVector Phase3HorrorScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	TSubclassOf<ASLMouseActor> Phase3MouseActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	float SweepAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	float SweepAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	float SweepAttackCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	TObjectPtr<UNiagaraSystem> SweepAttackEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Debug")
	bool bShowSweepDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	int32 SweepAttackHitCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 Settings")
	float SweepAttackHitInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 5 Settings")
	float DefaultWallStunDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 5 Settings")
	TObjectPtr<UNiagaraSystem> StunEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 5 Settings")
	TObjectPtr<USoundBase> StunSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Settings")
	float MoveToDropSpeed;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Settings")
	float DropHeight;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Settings")
	float InitialDropVelocity;
	
private:
	// Private Variables (Runtime State)
	EMouseActorState CurrentState;
	float CurrentHealth;
	UPROPERTY()
	TObjectPtr<ASLPlayerCharacter> TargetPlayer;
	FVector GrabTargetLocation;
	FVector MoveToOrbitTargetLocation;
	bool bCanGrab;
	float OrbitAngle;
	FVector OrbitCenter;

	bool bIsInPhase3Mode;
	bool bIsPlayerLookingAtMe;
	UPROPERTY()
	TObjectPtr<UStaticMesh> OriginalMesh;

	bool bCanSweepAttack;
	FTimerHandle SweepAttackCooldownTimer;

	FTimerHandle MultiHitTimer;
	int32 CurrentHitCount;
	UPROPERTY()
	TSet<TObjectPtr<AActor>> MultiHitTargets;

	bool bIsStunned;
	bool bIsAttackable;
	FTimerHandle StunRecoveryTimer;

	FVector DropTargetLocation;
	bool bIsMovingToDrop;
};