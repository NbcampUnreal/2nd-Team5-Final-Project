// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectTree.h"
#include "SLReactiveObjectTree.h"
#include "Kismet\GameplayStatics.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"
#include "StillLoading/Character/SLPlayerRidingCharacter.h"
#include "Components/SphereComponent.h"

ASLReactiveObjectTree::ASLReactiveObjectTree()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASLReactiveObjectTree::BeginOverlapCharacter);
}

void ASLReactiveObjectTree::SetStaticMesh(UStaticMesh* StaticMesh)
{
	StaticMeshComp->SetStaticMesh(StaticMesh);
}

void ASLReactiveObjectTree::BeginPlay()
{
	Super::BeginPlay();

	
}

void ASLReactiveObjectTree::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	return;
}
void ASLReactiveObjectTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move(DeltaTime);
}

void ASLReactiveObjectTree::BeginOverlapCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("RidingCharacter"))
	{
		if (ASLPlayerRidingCharacter* Character = Cast<ASLPlayerRidingCharacter>(OtherActor))
		{
			UGameplayStatics::ApplyDamage(Character, 1, nullptr, this, UDamageType::StaticClass());
		}
	}
}

void ASLReactiveObjectTree::Move(float DeltaTime)
{
	FVector MyPos = GetActorLocation();
	MyPos.X -= MoveSpeed * DeltaTime;
	SetActorLocation(MyPos);
}

