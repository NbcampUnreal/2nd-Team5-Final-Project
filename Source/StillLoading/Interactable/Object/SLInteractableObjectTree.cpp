// Fill out your copyright notice in the Description page of Project Settings.


#include "SLInteractableObjectTree.h"
#include "Kismet\GameplayStatics.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"
#include "StillLoading/Character/SLPlayerRidingCharacter.h"
#include "Components/SphereComponent.h"

ASLInteractableObjectTree::ASLInteractableObjectTree()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ASLInteractableObjectTree::SetStaticMesh(UStaticMesh* StaticMesh)
{
	StaticMeshComp->SetStaticMesh(StaticMesh);
}

void ASLInteractableObjectTree::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::BeginOverlapCharacter);
}

void ASLInteractableObjectTree::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	return;
}
void ASLInteractableObjectTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move(DeltaTime);
}

void ASLInteractableObjectTree::BeginOverlapCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("RidingCharacter"))
	{
		if (ASLPlayerRidingCharacter* Character = Cast<ASLPlayerRidingCharacter>(OtherActor))
		{
			UGameplayStatics::ApplyDamage(Character, 20, nullptr, this, UDamageType::StaticClass());
		}
	}
}

void ASLInteractableObjectTree::Move(float DeltaTime)
{
	FVector MyPos = GetActorLocation();
	MyPos.X -= MoveSpeed * DeltaTime;
	SetActorLocation(MyPos);
}

