#include "SLDefaultSword.h"

#include "Components/BoxComponent.h"

ASLDefaultSword::ASLDefaultSword()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionSphere"));
	BoxComponent->SetupAttachment(ItemMesh);

	BoxComponent->SetCollisionObjectType(ECC_GameTraceChannel1);

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetGenerateOverlapEvents(false);

	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASLDefaultSword::EnableOverlap()
{
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetGenerateOverlapEvents(true);
	HitActors.Empty();
}

void ASLDefaultSword::DisableOverlap()
{
	BoxComponent->SetGenerateOverlapEvents(false);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitActors.Empty();
}

void ASLDefaultSword::BeginPlay()
{
	Super::BeginPlay();

	BindOverlap(BoxComponent);
}
