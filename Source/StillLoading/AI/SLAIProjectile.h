// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "GameFramework/Actor.h"
#include "SLAIProjectile.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS(BlueprintType, Blueprintable)
class STILLLOADING_API ASLAIProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASLAIProjectile();

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	FORCEINLINE void SetAttackAnimType(EAttackAnimType NewAnimType) { AttackAnimType = NewAnimType; }

	void SetupSpawnedProjectile(EAttackAnimType AnimType, float Speed);
protected:
	// --- Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> CollisionComp;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
    
	// 발사체 트레일
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	// --- Effect Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> HitEffect;

	// 충돌 시 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> HitSound;

	// --- Damage Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	EAttackAnimType AttackAnimType;

private:
	// 내부 함수들
	void PlayHitEffects(const FHitResult& Hit);
	void ProcessDamage(AActor* HitActor, const FHitResult& Hit);
	void DestroyProjectileWithDelay(float DelayTime = 0.1f);
};
