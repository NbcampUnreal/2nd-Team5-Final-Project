#include "SLDefaultShield.h"

#include "Components/BoxComponent.h"

ASLDefaultShield::ASLDefaultShield()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionSphere"));
	BoxComponent->SetupAttachment(ItemMesh);

	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASLDefaultShield::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLDefaultShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

