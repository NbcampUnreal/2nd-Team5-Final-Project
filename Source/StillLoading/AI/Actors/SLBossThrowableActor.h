#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLBossThrowableActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USphereComponent;
class UStaticMeshComponent;
class USoundBase;
class ASLAIBaseCharacter;
class UProjectileMovementComponent;

UCLASS()
class STILLLOADING_API ASLBossThrowableActor : public AActor
{
    GENERATED_BODY()

public:
    ASLBossThrowableActor();

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetDamageAmount(float NewDamage);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetAttackType(EAttackAnimType NewAttackType);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComponent; }

protected:
    virtual void BeginPlay() override;

    void ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult);
    void PlayCharacterHitEffects(const FVector& HitLocation);
    void PlayGroundHitEffects(const FVector& HitLocation);
    void DestroyThrowable();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Character")
    TObjectPtr<UNiagaraSystem> CharacterHitEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Character")
    TObjectPtr<USoundBase> CharacterHitSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Ground")
    TObjectPtr<UNiagaraSystem> GroundHitEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Ground")
    TObjectPtr<USoundBase> GroundHitSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float DamageAmount;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    EAttackAnimType AttackType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    bool bDestroyOnHit;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float DestroyDelay;

private:
    UPROPERTY()
    TSet<AActor*> HitActors;
};