// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectTree.h"
#include "SLReactiveObjectTree.h"
#include "Kismet\GameplayStatics.h"
#include "StillLoading\Character\SLPlayerCharacterBase.h"

ASLReactiveObjectTree::ASLReactiveObjectTree()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASLReactiveObjectTree::OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	return;
}
void ASLReactiveObjectTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();
}

void ASLReactiveObjectTree::BeginOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Character"))
	{
		if (ASLPlayerCharacterBase* Character = Cast<ASLPlayerCharacterBase>(OtherActor))
		{
			//5분의 1씩 깎아서 5대 맞으면 죽게 설계해야함
			//Character->TakeDamage()
		}
	}
}

void ASLReactiveObjectTree::Move()
{
	FVector MyPos = GetActorLocation();
	MyPos.X += MoveSpeed;
	SetActorLocation(MyPos);
}

