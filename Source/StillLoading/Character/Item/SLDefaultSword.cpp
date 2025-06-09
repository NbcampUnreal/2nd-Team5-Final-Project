#include "SLDefaultSword.h"

#include "Components/BoxComponent.h"

ASLDefaultSword::ASLDefaultSword()
{
	PrimaryActorTick.bCanEverTick = true;

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
}

void ASLDefaultSword::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

int32 ASLDefaultSword::GetMaterialIndexFromGauge(const float Gauge) const
{
	const int32 Step = FMath::Clamp(static_cast<int32>(Gauge / 20.f), 0, 4);
	return Step;
}

void ASLDefaultSword::UpdateMaterialByGauge(const float Gauge)
{
	const int32 Index = GetMaterialIndexFromGauge(Gauge);

	UE_LOG(LogTemp, Warning, TEXT("Index: [%d], Gauge: [%f]"), Index, Gauge);

	if (SwordMaterials.IsValidIndex(Index) && ItemMesh)
	{
		ItemMesh->SetMaterial(0, SwordMaterials[Index]);
	}
}