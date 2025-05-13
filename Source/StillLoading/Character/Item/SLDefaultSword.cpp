#include "SLDefaultSword.h"

#include "Components/BoxComponent.h"

ASLDefaultSword::ASLDefaultSword()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionSphere"));
	BoxComponent->SetupAttachment(ItemMesh);

	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASLDefaultSword::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASLDefaultSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

