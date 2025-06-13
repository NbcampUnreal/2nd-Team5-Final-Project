// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLMinigameTreeSpawnManager.h"
#include "SLMinigameTreeSpawnManager.h"
#include "Components\BoxComponent.h"
#include "Interactable/Object/SLInteractableObjectTree.h"

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
		ASLInteractableObjectTree* Tree = GetWorld()->SpawnActor<ASLInteractableObjectTree>(TreeRef);
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
	ASLInteractableObjectTree* Tree = *It;
	It.RemoveCurrent();
	
	int RandLineIndex = FMath::RandRange(0, LineIndex - 1);
	Tree->SetActorLocation(SpawnLocationArray[RandLineIndex]);
	int RandMeshIndex = FMath::RandRange(0, MeshComponents.Num() - 1);
	Tree->SetStaticMesh(MeshComponents[RandMeshIndex]);
	Tree->SetActorHiddenInGame(false);
	Tree->SetActorTickEnabled(true);
	Tree->SetActorEnableCollision(true);
}

void ASLMinigameTreeSpawnManager::BeginOverlapTree(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Tree"))
	{
		if (ASLInteractableObjectTree* Tree = Cast<ASLInteractableObjectTree>(OtherActor))
		{
			Tree->SetActorHiddenInGame(true);
			Tree->SetActorTickEnabled(false);
			Tree->SetActorEnableCollision(false);
			TreeSet.Add(Tree);
		}
	}
}


