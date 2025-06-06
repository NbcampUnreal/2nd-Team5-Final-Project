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

	/*
	if (EmpoweredNiagaraEffect)
	{
		EmpoweredEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			EmpoweredNiagaraEffect,
			GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale,
			true
		);

		if (EmpoweredEffectComponent)
		{
			float ChargingRatio = 0.5f;
			EmpoweredEffectComponent->SetVariableFloat(FName("ChargingPower"), ChargingRatio);
		}
	}
	*/
	//BindOverlap(BoxComponent);
}

void ASLDefaultSword::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}