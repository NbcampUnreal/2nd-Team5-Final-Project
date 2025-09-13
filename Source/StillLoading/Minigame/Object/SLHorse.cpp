// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLHorse.h"
#include "Components/SplineComponent.h"

// Sets default values
ASLHorse::ASLHorse()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASLHorse::BeginPlay()
{
	Super::BeginPlay();

	TargetSpline = TargetSplineActor->FindComponentByClass<USplineComponent>();
	if (TargetSpline)
	{
		UE_LOG(LogTemp, Log, TEXT("Spline Points: %d"), TargetSpline->GetNumberOfSplinePoints());
	}

	GetCharacterMovement()->MaxWalkSpeed = AutoSpeed;
}

// Called every frame
void ASLHorse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();
}

void ASLHorse::Move()
{
	if (!bShouldMove)
	{
		return;
	}

	if (TargetSplineActor)
	{
		FVector ActorLocation = GetActorLocation();
		float InputKey = TargetSpline->FindInputKeyClosestToWorldLocation(ActorLocation);
		CurrentDistance = TargetSpline->GetDistanceAlongSplineAtSplineInputKey(InputKey);

		// 스플라인 진행 방향
		FVector SplineDir = TargetSpline->GetDirectionAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		FRotator Rotation = TargetSpline->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

		AddMovementInput(SplineDir, 1.0f);
		SetActorRotation(Rotation);
	}
}


void ASLHorse::OnEnemyDetected()
{
	OverlapPawnCount++;
	bShouldMove = false;

}

void ASLHorse::OnEnemyCleared()
{
	OverlapPawnCount = FMath::Max(OverlapPawnCount - 1, 0);
	if (OverlapPawnCount == 0)
	{
		bShouldMove = true;

	}
}

void ASLHorse::OnTrigger()
{
	bShouldMove = true;

}



