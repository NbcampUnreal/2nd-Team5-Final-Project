// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLRunnerHurdle.h"
#include "Components/BoxComponent.h"
#include "Kismet\GameplayStatics.h"

// Sets default values
ASLRunnerHurdle::ASLRunnerHurdle()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
}

// Called when the game starts or when spawned
void ASLRunnerHurdle::BeginPlay()
{
	Super::BeginPlay();
}


