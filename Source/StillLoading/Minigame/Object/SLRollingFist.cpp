// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLRollingFist.h"

ASLRollingFist::ASLRollingFist()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	ReturnPosition = FVector::ZeroVector;
	TargetPosition = FVector::ZeroVector;
	RotationSpeed = 5000.0f;
}

void ASLRollingFist::BeginPlay()
{
	Super::BeginPlay();

	TargetPosition = GetActorLocation() + TargetPosition;
	ReturnPosition = GetActorLocation() + ReturnPosition;

}


void ASLRollingFist::Tick(float DeltaTime)
{
	if (bIsTrigger)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Target;

		if (bArrivalTargetPoint)
		{
			Target = ReturnPosition;
		}
		else
		{
			Target = TargetPosition;
		}
		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, Target, DeltaTime, Speed);
		SetActorLocation(NewLocation);

		AddActorLocalRotation(FRotator(-RotationSpeed * DeltaTime, 0.0f, 0.0f));


		if (FVector::Dist(NewLocation, Target) < 1.0f)
		{
			if (bArrivalTargetPoint)
			{
				DeActivate();
			}
			else
			{
				bArrivalTargetPoint = true;
			}

		}






	}
}

void ASLRollingFist::Activate()
{
	Super::Activate();
	bIsTrigger = true;
	SetActorTickEnabled(true);
}

void ASLRollingFist::DeActivate()
{

	bIsTrigger = true;
	SetActorTickEnabled(false);
}

