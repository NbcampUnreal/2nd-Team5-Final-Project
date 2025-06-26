// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLDungeonDoor.h"

#include "Kismet/GameplayStatics.h"


ASLDungeonDoor::ASLDungeonDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	OpenPosition = FVector::ZeroVector;

}

bool ASLDungeonDoor::GetIsOpened() const
{
	return bIsOpen;
}

void ASLDungeonDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsTrigger)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Target;

		if (bIsOpen)
		{
			Target = OpenPosition;
		}
		else
		{
			Target = ClosedPosition;
		}
		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, Target, DeltaTime, SlideSpeed);
		SetActorLocation(NewLocation);

		if (FVector::Dist(NewLocation, Target) < 1.0f)
		{
			bIsTrigger = false;
			SetActorTickEnabled(false);
		}
	}
}

void ASLDungeonDoor::Activate()
{
	Super::Activate();
	bIsTrigger = true;
	bIsOpen = true;
	UGameplayStatics::PlaySound2D(this, DoorSound, 1.0f);
	SetActorTickEnabled(true);
	
}

void ASLDungeonDoor::DeActivate()
{
	Super::DeActivate();

	bIsTrigger = true;
	bIsOpen = false;
	UGameplayStatics::PlaySound2D(this, DoorSound, 1.0f);
	SetActorTickEnabled(true);

}

void ASLDungeonDoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedPosition = GetActorLocation();
	OpenPosition = ClosedPosition + OpenPosition;
}
