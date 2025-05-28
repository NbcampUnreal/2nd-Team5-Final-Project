// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLBackgroundLooper.h"

// Sets default values
ASLBackgroundLooper::ASLBackgroundLooper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

}

// Called when the game starts or when spawned
void ASLBackgroundLooper::BeginPlay()
{
	Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Tiles.Num = %d"), Tiles.Num());
}

// Called every frame
void ASLBackgroundLooper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    FVector MoveOffset = FVector(-ScrollSpeed * DeltaTime, 0.0f, 0.0f);

    for (AActor* Tile : Tiles)
    {
        if (Tile && IsValid(Tile))
        {
            Tile->AddActorWorldOffset(MoveOffset);
            CheckAndResetTile(Tile);
        }
    }
}

void ASLBackgroundLooper::CheckAndResetTile(AActor* Tile)
{

    if (!Tile) return;

    FVector Location = Tile->GetActorLocation();

    if (Location.X < -TileLength)
    {
        // 가장 오른쪽 타일의 X 위치 계산
        float MaxX = -FLT_MAX;
        for (AActor* Other : Tiles)
        {
            if (Other && Other != Tile)
            {
                MaxX = FMath::Max(MaxX, Other->GetActorLocation().X);
            }
        }

        FVector NewLocation = Location;
        NewLocation.X = MaxX + TileLength;
        Tile->SetActorLocation(NewLocation);
    }
}

