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
#include "AI/Companion/SLCompanionCharacter.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"


ASLAIProjectile::ASLAIProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// 구체 충돌 컴포넌트 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	
	//CollisionComp->OnComponentHit.AddDynamic(this, &ASLAIProjectile::OnHit);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASLAIProjectile::OnComponentBeginOverlap);
	
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);           // 캐릭터와 충돌
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);    // 월드와 충돌
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);   // 동적 오브젝트와 충돌
	CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	RootComponent = CollisionComp;

	
	// 발사체 이동 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 2500.f;
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

	TeamId = FGenericTeamId::NoTeam;
	bHasHitTarget = false;
}


void ASLAIProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bDebugHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== PROJECTILE HIT DEBUG ==="));
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
		UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), OtherComp ? *OtherComp->GetName() : TEXT("NULL"));
		UE_LOG(LogTemp, Warning, TEXT("Instigator: %s"), GetInstigator() ? *GetInstigator()->GetName() : TEXT("NULL"));
		UE_LOG(LogTemp, Warning, TEXT("Owner: %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));
	}
    
	// 다른 프로젝타일과 충돌했는지 확인
	if (OtherActor && Cast<ASLAIProjectile>(OtherActor))
	{
		// 다른 프로젝타일과 충돌한 경우 아무 작업도 수행하지 않고 반환
		return;
	}

	if (OtherActor && TeamId != FGenericTeamId::NoTeam)
	{
		if (APawn* HitPawn = Cast<APawn>(OtherActor))
		{
			if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(HitPawn->GetController()))
			{
				if (TeamAgent->GetGenericTeamId() == TeamId)
				{
					// 같은 팀이면 충돌 무시하고 관통
					CollisionComp->IgnoreActorWhenMoving(OtherActor, true);
					return;
				}
			}
		}
	}
	
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
	//ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
}

void ASLAIProjectile::BeginPlay()
{
	Super::BeginPlay();
    
	// 발사한 캐릭터와의 충돌 무시 설정
	if (GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
        
		// 캐릭터의 모든 컴포넌트와 충돌 무시
		TArray<UPrimitiveComponent*> Components;
		GetInstigator()->GetComponents<UPrimitiveComponent>(Components);
		for (UPrimitiveComponent* Component : Components)
		{
			CollisionComp->IgnoreComponentWhenMoving(Component, true);
		}

		CollisionComp->MoveIgnoreActors.Add(GetInstigator());
		if (UPrimitiveComponent* InstigatorRoot = Cast<UPrimitiveComponent>(GetInstigator()->GetRootComponent()))
		{
			InstigatorRoot->MoveIgnoreActors.Add(this);
		}

		IgnoreSameTeamActors();
	}
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

void ASLAIProjectile::SetTeamId(const FGenericTeamId& NewTeamId)
{
	TeamId = NewTeamId;
}

FGenericTeamId ASLAIProjectile::GetTeamId() const
{
	return TeamId;
}

void ASLAIProjectile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator())
	{
		return;
	}
    
	// 이미 히트 처리가 완료된 경우
	if (bHasHitTarget)
	{
		return;
	}
    
	// 이미 히트한 액터인지 확인
	if (AlreadyHitActors.Contains(OtherActor))
	{
		return;
	}
    
	// 다른 프로젝타일과 충돌했는지 확인
	if (Cast<ASLAIProjectile>(OtherActor))
	{
		return;
	}
    
	// 팀 체크 - 같은 팀이면 무시
	if (TeamId != FGenericTeamId::NoTeam)
	{
		if (APawn* HitPawn = Cast<APawn>(OtherActor))
		{
			if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(HitPawn->GetController()))
			{
				if (TeamAgent->GetGenericTeamId() == TeamId)
				{
					// 같은 팀이면 무시
					return;
				}
			}
		}
	}
    
	// 히트한 액터 추가
	AlreadyHitActors.Add(OtherActor);
    
	// 충돌 처리
	FHitResult HitResult;
	if (bFromSweep)
	{
		HitResult = SweepResult;
	}
	else
	{
		HitResult.Location = GetActorLocation();
		HitResult.ImpactPoint = GetActorLocation();
		HitResult.ImpactNormal = (GetActorLocation() - OtherActor->GetActorLocation()).GetSafeNormal();
	}
    
	// 충돌 이펙트 재생
	PlayHitEffects(HitResult);
    
	// 데미지 처리
	ProcessDamage(OtherActor, HitResult);
    
	// 히트 처리 완료 표시
	bHasHitTarget = true;
    
	// 즉시 충돌 비활성화
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
	// 트레일 이펙트 중지
	if (NiagaraComponent)
	{
		NiagaraComponent->Deactivate();
	}
    
	// 딜레이 후 발사체 제거
	DestroyProjectileWithDelay(0.1f);
}

void ASLAIProjectile::IgnoreSameTeamActors()
{
	if (TeamId == FGenericTeamId::NoTeam)
	{
		return;
	}
    
	// 월드의 모든 Pawn 검색
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), FoundActors);
    
	for (AActor* Actor : FoundActors)
	{
		if (APawn* Pawn = Cast<APawn>(Actor))
		{
			if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Pawn->GetController()))
			{
				if (TeamAgent->GetGenericTeamId() == TeamId)
				{
					CollisionComp->IgnoreActorWhenMoving(Actor, true);
				}
			}
		}
	}
}