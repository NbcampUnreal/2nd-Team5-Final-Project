// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameTreeSpawnManager.h"
#include "SLMinigameTreeSpawnManager.h"
#include "Components\BoxComponent.h"
#include "StillLoading\Minigame\Object\SLReactiveObjectTree.h"

// Sets default values
ASLMinigameTreeSpawnManager::ASLMinigameTreeSpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	FinishBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));


	FinishBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASLMinigameTreeSpawnManager::BeginOverlapTree);
}

// Called when the game starts or when spawned
void ASLMinigameTreeSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	CreateTree(BeginSpawnCount);
}

void ASLMinigameTreeSpawnManager::CreateTree(int SpawnCount)
{
	for (int i = 0; i < SpawnCount; i++)
	{
		ASLReactiveObjectTree* Tree = GetWorld()->SpawnActor<ASLReactiveObjectTree>(TreeRef);
		Tree->SetActorHiddenInGame(true);
		Tree->SetActorTickEnabled(false);
		Tree->SetActorEnableCollision(false);
		TreeSet.Add(Tree);
	}
}

void ASLMinigameTreeSpawnManager::SpawnTree()
{
	if (TreeSet.IsEmpty())
	{
		CreateTree(5);
	}
	auto It = TreeSet.CreateIterator();
	ASLReactiveObjectTree* Tree = *It;
	It.RemoveCurrent();

	int RandIndex = FMath::RandRange(0, LineIndex);
	if (!SpawnLocationArray.IsValidIndex(RandIndex)) return;
	Tree->SetActorLocation(SpawnLocationArray[RandIndex]);

	Tree->SetActorHiddenInGame(false);
	Tree->SetActorTickEnabled(true);
	Tree->SetActorEnableCollision(true);
}

void ASLMinigameTreeSpawnManager::BeginOverlapTree(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Tree"))
	{
		if (ASLReactiveObjectTree* Tree = Cast<ASLReactiveObjectTree>(OtherActor))
		{
			Tree->SetActorHiddenInGame(true);
			Tree->SetActorTickEnabled(false);
			Tree->SetActorEnableCollision(false);
			TreeSet.Add(Tree);
		}
	}
}


