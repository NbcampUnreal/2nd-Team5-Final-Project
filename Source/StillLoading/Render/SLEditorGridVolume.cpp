// Fill out your copyright notice in the Description page of Project Settings.


#include "SLEditorGridVolume.h"

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

	const UWorld* World = GetWorld();
	if (!World) return;

	const float PitchRadians = FMath::DegreesToRadians(CameraFitch);
	const float HeightScale = 1.f / FMath::Cos(PitchRadians);

	const float CellWidth = static_cast<float>(GridHeight) * HeightScale;
	const float CellHeight = static_cast<float>(GridWidth);

	const int32 HalfCount = GridCount / 2;

	const FVector GridOffset = FVector(-HalfCount * CellWidth + CellWidth * 0.5f,
								 -HalfCount * CellHeight + CellHeight * 0.5f,
								 0.f);
	
	for (int32 X = 0; X < GridCount; ++X)
	{
		for (int32 Y = 0; Y < GridCount; ++Y)
		{
			FVector Start = GridOffset + FVector(X * CellWidth, Y * CellHeight, GroundHeight);
			FVector EndX = Start + FVector(CellWidth, 0.f, 0.f);
			FVector EndY = Start + FVector(0.f, CellHeight, 0.f);

			DrawDebugLine(World, Start, EndX, GridColor, false, -1.f, SDPG_Foreground, LineThickness);
			DrawDebugLine(World, Start, EndY, GridColor, false, -1.f, SDPG_Foreground, LineThickness);
		}
	}


#endif
}
