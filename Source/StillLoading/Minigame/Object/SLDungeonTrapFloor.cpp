// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLDungeonTrapFloor.h"

ASLDungeonTrapFloor::ASLDungeonTrapFloor()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrnetVisibleTime = 0.0f;
}

void ASLDungeonTrapFloor::Activate()
{
	ShowMesh();

}

void ASLDungeonTrapFloor::DeActivate()
{
	HideMesh();
}

void ASLDungeonTrapFloor::BeginPlay()
{
	Super::BeginPlay();

	HideMesh();
}

void ASLDungeonTrapFloor::HideMesh()
{
	SetActorHiddenInGame(true);

	if (StaticMeshComp)
	{
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorEnableCollision(false);

	SetActorTickEnabled(false);

	CurrnetVisibleTime = 0;

}

void ASLDungeonTrapFloor::ShowMesh()
{
	SetActorHiddenInGame(false);

	if (StaticMeshComp)
	{
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	SetActorEnableCollision(true);

	CurrnetVisibleTime = 0;

	SetActorTickEnabled(true);
}
