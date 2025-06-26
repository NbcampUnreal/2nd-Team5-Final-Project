#include "ArrowProjectile.h"

#include "GenericTeamAgentInterface.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AArrowProjectile::AArrowProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = CollisionComp;
	CollisionComp->InitSphereRadius(15.0f);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SpearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	SpearMesh->SetupAttachment(RootComponent);
	SpearMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = 2500.f;
	ProjectileMovement->MaxSpeed = 2500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.2f;

	InitialLifeSpan = 15.0f;
}

void AArrowProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AArrowProjectile::OnBeginOverlap);
}

void AArrowProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || HitActors.Contains(OtherActor))
	{
		return;
	}

	HitActors.Add(OtherActor);

	if (UBattleComponent* BattleComp = OtherActor->FindComponentByClass<UBattleComponent>())
	{
		const IGenericTeamAgentInterface* OwningTeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner());
		const IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);

		if (OwningTeamAgent && TargetTeamAgent)
		{
			if (OwningTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId())
			{
				BattleComp->ReceiveHitResult(10, OtherActor, SweepResult, EAttackAnimType::AAT_AISpecial);
			}
		}
	}
}

