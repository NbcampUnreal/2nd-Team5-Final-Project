// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLDungeonActivator.h"

// Sets default values
ASLDungeonActivator::ASLDungeonActivator()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMeshComp->SetupAttachment(SceneComp);
}

void ASLDungeonActivator::BeginPlay()
{
	Super::BeginPlay();

}

void ASLDungeonActivator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASLDungeonActivator::Activate()
{
}

void ASLDungeonActivator::DeActivate()
{
}

