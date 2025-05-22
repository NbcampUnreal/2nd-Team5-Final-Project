// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DataAsset/AttackDataAsset.h"
#include "GameFramework/Actor.h"
#include "SLAIProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class STILLLOADING_API ASLAIProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASLAIProjectile();

	// 발사체가 대상에 명중했을 때 호출
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// 충돌 감지를 위한 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComp;
    
	// 발사체 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
    
	// 파티클 시스템 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystemComponent> ParticleSystem;
    
	// 데미지 양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	EAttackAnimType AttackAnimType;
};
