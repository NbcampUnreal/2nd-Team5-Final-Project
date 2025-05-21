// Fill out your copyright notice in the Description page of Project Settings.


#include "SLPixelPerfectCamera.h"

USLPixelPerfectCamera::USLPixelPerfectCamera()
{
	bConstrainAspectRatio = true;
	AspectRatio = 16.0f / 9.0f;
}

void USLPixelPerfectCamera::BeginPlay()
{
	Super::BeginPlay();
	OwnerActor = GetOwner();

	InitPixelSize();
	SetTickGroup(TG_PostPhysics);
}

void USLPixelPerfectCamera::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsValid( OwnerActor))
	{
		FVector TargetLocation = OwnerActor->GetActorLocation();
		FVector CurrentSnapLocation = FVector::ZeroVector;
		
		CurrentSnapLocation.X = FMath::GridSnap(TargetLocation.X, PixelSize.X);
		CurrentSnapLocation.Y = FMath::GridSnap(TargetLocation.Y, PixelSize.Y);
		SetWorldLocation(CurrentSnapLocation);
	}
}