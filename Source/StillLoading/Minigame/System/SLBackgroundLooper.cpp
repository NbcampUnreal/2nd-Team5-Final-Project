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
           
        }
    }
    CheckAndResetTile();
}

void ASLBackgroundLooper::CheckAndResetTile()
{

    for (AActor* Tile : Tiles)
    {
        if (!IsValid(Tile)) continue;

        FVector Location = Tile->GetActorLocation();

        // 왼쪽 경계 넘어갔을 때 처리
        if (Location.X < -TileLength)
        {
            // 가장 오른쪽에 있는 타일 찾기
            float MaxX = -FLT_MAX;

            for (AActor* Other : Tiles)
            {
                if (Other && Other != Tile)
                {
                    float X = Other->GetActorLocation().X;

                    // 부동소수점 보정
                    X = FMath::GridSnap(X, TileLength);

                    MaxX = FMath::Max(MaxX, X);
                }
            }

            // 새로운 위치 계산: MaxX + TileLength
            FVector NewLocation = Location;
            NewLocation.X = MaxX + TileLength;

            // 다시 스냅 (보정)
            NewLocation.X = FMath::GridSnap(NewLocation.X, TileLength);

            Tile->SetActorLocation(NewLocation);

            // 디버그 로그
            UE_LOG(LogTemp, Warning, TEXT("Moved tile to %.2f"), NewLocation.X);
           
        }
    }
}

