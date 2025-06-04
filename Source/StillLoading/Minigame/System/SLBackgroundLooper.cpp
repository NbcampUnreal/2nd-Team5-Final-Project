// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/System/SLBackgroundLooper.h"
#include "TimerManager.h"

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
	float Interval = TileLength / ScrollSpeed;

	// 일정 시간마다 MoveLeftmostTile 호출
	GetWorldTimerManager().SetTimer(TileLoopTimerHandle, this, &ASLBackgroundLooper::MoveLeftmostTile, Interval, true);
}

void ASLBackgroundLooper::MoveLeftmostTile()
{
	AActor* LeftmostTile = GetLeftTile();

	if (!IsValid(LeftmostTile)) return;


	float MaxX = -FLT_MAX;
	for (AActor* Tile : Tiles)
	{
		if (IsValid(Tile) && Tile != LeftmostTile)
		{
			MaxX = FMath::Max(MaxX, Tile->GetActorLocation().X);
		}
	}

	FVector NewLocation = LeftmostTile->GetActorLocation();
	NewLocation.X = MaxX + TileLength;
	LeftmostTile->SetActorLocation(NewLocation);
}

TObjectPtr<AActor> ASLBackgroundLooper::GetLeftTile()
{
	AActor* Result = nullptr;
	float MinX = FLT_MAX;

	for (AActor* Tile : Tiles)
	{
		if (IsValid(Tile))
		{
			float TileX = Tile->GetActorLocation().X;
			if (TileX < MinX)
			{
				MinX = TileX;
				Result = Tile;
			}
		}
	}

	return Result;
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

		}
	}
}
