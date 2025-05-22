#include "SLAIProjectile.h"

#include "Character/SLAIBaseCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"


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
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 없음

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetAutoActivate(true);

	
	// 기본값 설정
	HitEffect = nullptr;
	HitSound = nullptr;

	// 5초 후 자동 소멸
	InitialLifeSpan = 5.0f;
}


void ASLAIProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherActor != GetInstigator() && OtherComp)
	{
		// 충돌 이펙트 재생
		PlayHitEffects(Hit);

		// 데미지 처리
		ProcessDamage(OtherActor, Hit);

		// 트레일 이펙트 중지
		if (NiagaraComponent)
		{
			NiagaraComponent->Deactivate();
		}

		// 딜레이 후 발사체 제거
		DestroyProjectileWithDelay(0.1f);
	}
}

void ASLAIProjectile::SetupSpawnedProjectile(EAttackAnimType AnimType, float Speed)
{
	AttackAnimType = AnimType;
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
}

void ASLAIProjectile::PlayHitEffects(const FHitResult& Hit)
{
	// 충돌 지점에 나이아가라 이펙트 재생
	if (HitEffect)
	{
		FRotator EffectRotation = Hit.Normal.Rotation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), 
			HitEffect, 
			Hit.Location, 
			EffectRotation,
			FVector(1.0f), // 스케일
			true,          // 자동 소멸
			true,          // 자동 활성화
			ENCPoolMethod::AutoRelease
		);
	}

	// 충돌 사운드 재생
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			HitSound,
			Hit.Location,
			1.0f,  // 볼륨
			1.0f   // 피치
		);
	}
}

void ASLAIProjectile::ProcessDamage(AActor* HitActor, const FHitResult& Hit)
{
	if (ASLAIBaseCharacter* OwnerCharacter = Cast<ASLAIBaseCharacter>(GetInstigator()))
	{
		OwnerCharacter->GetBattleComponent()->SendHitResult(HitActor, Hit, AttackAnimType);
	}
}

void ASLAIProjectile::DestroyProjectileWithDelay(float DelayTime)
{
	// 약간의 딜레이 후 발사체 제거 (이펙트가 재생될 시간을 줌)
	FTimerHandle DestroyTimer;
	GetWorldTimerManager().SetTimer(DestroyTimer, [this]()
	{
		Destroy();
	}, DelayTime, false);
}