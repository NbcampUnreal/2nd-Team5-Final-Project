// Fill out your copyright notice in the Description page of Project Settings.


#include "SLEditorGridVolume.h"


// Sets default values
ASLEditorGridVolume::ASLEditorGridVolume()
{
	PrimaryActorTick.bCanEverTick = false;
#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	bIsEditorOnlyActor = true;
#endif
}

void ASLEditorGridVolume::ShowGrid()
{
	GridVisible = true;
}

void ASLEditorGridVolume::HideGrid()
{
	GridVisible = false;
}

void ASLEditorGridVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#if WITH_EDITOR
	if (!GridVisible) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector Origin = GetActorLocation();

	// 절반 칸 수
	int32 HalfCount = GridCount / 2;

	// 시작 지점을 왼쪽 아래로 오프셋
	FVector GridOffset = FVector(-HalfCount * GridWidth + GridWidth * 0.5f,
								 -HalfCount * GridHeight + GridHeight * 0.5f,
								 0.f);

	GridOffset.X += GridOffsetX;
	
	for (int32 X = 0; X < GridCount; ++X)
	{
		for (int32 Y = 0; Y < GridCount; ++Y)
		{
			// X: 가로로 GridWidth 간격, Y: 세로로 GridHeight 간격
			FVector Start = Origin + GridOffset + FVector(X * GridWidth, Y * GridHeight, 0.f);
			FVector EndX = Start + FVector(0, GridHeight, 0); // 위쪽 선 (Y 방향)
			FVector EndY = Start + FVector(GridWidth, 0, 0);  // 오른쪽 선 (X 방향)

			// 선을 바르게 그리기 (X: 위쪽, Y: 오른쪽)
			DrawDebugLine(World, Start, EndX, GridColor, false, -1.f, 0, LineThickness);
			DrawDebugLine(World, Start, EndY, GridColor, false, -1.f, 0, LineThickness);
		}
	}


#endif
}

void ASLEditorGridVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector Location = GetActorLocation();
	Location.Z = 100.0f;
	SetActorLocation(Location);
}
