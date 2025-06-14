#include "SLBossThrowableActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASLBossThrowableActor::ASLBossThrowableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->SetSphereRadius(50.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
    CollisionComponent->SetNotifyRigidBodyCollision(true);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CollisionComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
    ProjectileMovementComponent->InitialSpeed = 0.0f;
    ProjectileMovementComponent->MaxSpeed = 3000.0f;
    ProjectileMovementComponent->bAutoActivate = false;

    DamageAmount = 50.0f;
    AttackType = EAttackAnimType::AAT_Attack_01;
    bDestroyOnHit = true;
    DestroyDelay = 0.5f;
}

void ASLBossThrowableActor::BeginPlay()
{
    Super::BeginPlay();

    CollisionComponent->OnComponentHit.AddDynamic(this, &ASLBossThrowableActor::OnHit);
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASLBossThrowableActor::OnOverlapBegin);

    if (AActor* OwnerActor = GetOwner())
    {
        CollisionComponent->IgnoreActorWhenMoving(OwnerActor, true);
        
        TArray<UPrimitiveComponent*> OwnerComponents;
        OwnerActor->GetComponents<UPrimitiveComponent>(OwnerComponents);
        for (UPrimitiveComponent* Component : OwnerComponents)
        {
            CollisionComponent->IgnoreComponentWhenMoving(Component, true);
        }
    }
}

void ASLBossThrowableActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    if (HitActors.Contains(OtherActor))
    {
        return;
    }

    HitActors.Add(OtherActor);

    // Pawn(캐릭터)에 충돌한 경우
    if (OtherActor->IsA<APawn>())
    {
        ApplyDamageToTarget(OtherActor, Hit);
        PlayCharacterHitEffects(Hit.Location);
        
        if (bDestroyOnHit)
        {
            DestroyThrowable();
        }
        return;
    }

    // 월드 오브젝트(바닥, 벽 등)에 충돌한 경우
    if (OtherComp && 
        (OtherComp->GetCollisionObjectType() == ECC_WorldStatic || 
         OtherComp->GetCollisionObjectType() == ECC_WorldDynamic))
    {
        PlayGroundHitEffects(Hit.Location);
        
        if (bDestroyOnHit)
        {
            DestroyThrowable();
        }
        return;
    }
}

void ASLBossThrowableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    if (!OtherActor->IsA<APawn>())
    {
        return;
    }

    if (HitActors.Contains(OtherActor))
    {
        return;
    }

    HitActors.Add(OtherActor);

    ApplyDamageToTarget(OtherActor, SweepResult);
    PlayCharacterHitEffects(SweepResult.Location);

    if (bDestroyOnHit)
    {
        DestroyThrowable();
    }
}

void ASLBossThrowableActor::ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult)
{
    if (!Target || !GetOwner())
    {
        return;
    }

    UBattleComponent* TargetBattleComp = Target->FindComponentByClass<UBattleComponent>();
    if (!TargetBattleComp)
    {
        return;
    }

    ASLAIBaseCharacter* OwnerCharacter = Cast<ASLAIBaseCharacter>(GetOwner());
    if (!OwnerCharacter || !OwnerCharacter->GetBattleComponent())
    {
        return;
    }

    OwnerCharacter->GetBattleComponent()->SendHitResult(Target, HitResult, AttackType);
}

void ASLBossThrowableActor::PlayCharacterHitEffects(const FVector& HitLocation)
{
    if (CharacterHitEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            CharacterHitEffect,
            HitLocation,
            FRotator::ZeroRotator,
            FVector(1.0f),
            true,
            true,
            ENCPoolMethod::None,
            true
        );
    }

    if (CharacterHitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            CharacterHitSound,
            HitLocation
        );
    }
}

void ASLBossThrowableActor::PlayGroundHitEffects(const FVector& HitLocation)
{
    if (GroundHitEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            GroundHitEffect,
            HitLocation,
            FRotator::ZeroRotator,
            FVector(1.0f),
            true,
            true,
            ENCPoolMethod::None,
            true
        );
    }

    if (GroundHitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            GroundHitSound,
            HitLocation
        );
    }
}

void ASLBossThrowableActor::DestroyThrowable()
{
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComponent->SetVisibility(false);

    FTimerHandle DestroyTimer;
    GetWorld()->GetTimerManager().SetTimer(
        DestroyTimer,
        [this]()
        {
            Destroy();
        },
        DestroyDelay,
        false
    );
}

void ASLBossThrowableActor::SetDamageAmount(float NewDamage)
{
    DamageAmount = NewDamage;
}

void ASLBossThrowableActor::SetAttackType(EAttackAnimType NewAttackType)
{
    AttackType = NewAttackType;
}