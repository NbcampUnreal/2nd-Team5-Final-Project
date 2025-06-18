#include "SLDeveloperBossLine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/SLPlayerCharacterBase.h"
#include "Engine/Engine.h"

ASLDeveloperBossLine::ASLDeveloperBossLine()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	LineMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineMeshComponent"));
	LineMeshComponent->SetupAttachment(RootComponent);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);

	LineMesh = nullptr;
	InactiveMaterial = nullptr;
	ActiveMaterial = nullptr;
}

void ASLDeveloperBossLine::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &ASLDeveloperBossLine::OnLineHit);
	}

	if (LineMesh && LineMeshComponent)
	{
		LineMeshComponent->SetStaticMesh(LineMesh);
	}
}

void ASLDeveloperBossLine::ActivateLine()
{
	ActivateActor();
}

void ASLDeveloperBossLine::DeactivateLine()
{
	DeactivateActor();
}

void ASLDeveloperBossLine::DestroyLine()
{
	DestroyActor();
}

void ASLDeveloperBossLine::SetLineIndex(int32 Index)
{
	SetActorIndex(Index);
}

int32 ASLDeveloperBossLine::GetLineIndex() const
{
	return GetActorIndex();
}

void ASLDeveloperBossLine::OnLineHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || !CanBeDestroyed())
	{
		return;
	}

	// 플레이어의 직접적인 물리 충돌 처리 (기존 방식 유지)
	if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(OtherActor))
	{
		float Damage = 50.0f;
		TakeDamage(Damage);
		UE_LOG(LogTemp, Log, TEXT("Boss Line %d damaged by collision"), GetLineIndex());
	}
}

void ASLDeveloperBossLine::UpdateActorVisuals()
{
	if (!LineMeshComponent)
	{
		return;
	}

	EDestructibleState State = GetCurrentState();

	switch (State)
	{
	case EDestructibleState::Inactive:
		LineMeshComponent->SetVisibility(false);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (InactiveMaterial)
		{
			LineMeshComponent->SetMaterial(0, InactiveMaterial);
		}
		break;

	case EDestructibleState::Active:
		LineMeshComponent->SetVisibility(true);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (ActiveMaterial)
		{
			LineMeshComponent->SetMaterial(0, ActiveMaterial);
		}
		break;

	case EDestructibleState::Destroyed:
		LineMeshComponent->SetVisibility(false);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void ASLDeveloperBossLine::OnActorDestroyed()
{
	Super::OnActorDestroyed();
	
	if (OnBossLineDestroyed.IsBound())
	{
		OnBossLineDestroyed.Broadcast(GetLineIndex());
	}
}