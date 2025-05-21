// Fill out your copyright notice in the Description page of Project Settings.


#include "SLAIProjectile.h"

#include "Character/SLAIBaseCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


ASLAIProjectile::ASLAIProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// 구체 충돌 컴포넌트 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASLAIProjectile::OnHit);
	RootComponent = CollisionComp;

	// 발사체 이동 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 없음 (마법 발사체)

	// 파티클 시스템 설정
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleSystem->SetupAttachment(RootComponent);
    
	// 기본 데미지 설정
	Damage = 20.0f;

	// 5초 후 자동 소멸
	InitialLifeSpan = 5.0f;
}


void ASLAIProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetInstigator() && OtherComp)
	{
		// 이걸 배틀 컴포넌트로 데미지 처리하면 될듯
		UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, GetInstigator(), nullptr);

		if (ASLAIBaseCharacter* OwnerCharacter = Cast<ASLAIBaseCharacter>(GetInstigator()))
		{
			OwnerCharacter->GetBattleComponent()->SendHitResult(OtherActor, Hit, AttackAnimType);
		}
        
		// 충돌 효과 (이펙트, 사운드 등)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), nullptr, Hit.Location);
        
		// 발사체 제거
		Destroy();
	}
}
