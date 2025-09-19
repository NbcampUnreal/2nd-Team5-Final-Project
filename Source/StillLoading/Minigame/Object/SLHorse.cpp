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

	GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
}

// Called every frame
void ASLHorse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move(DeltaTime);
}

void ASLHorse::Move(float DeltaTime)
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

		if (!bDeceleration)
		{
			Accelerate(DeltaTime);
		}

		else
		{
			Deceleration(DeltaTime);
		}

		AddMovementInput(SplineDir, 1.0f);
		SetActorRotation(Rotation);
	}

}


void ASLHorse::OnEnemyDetected()
{
	OverlapEnemyCount++;
	bDeceleration = true;
}

void ASLHorse::OnEnemyCleared()
{
	OverlapEnemyCount = FMath::Max(OverlapEnemyCount - 1, 0);
	if (OverlapEnemyCount == 0)
	{
		bShouldMove = true;
		bDeceleration = false;
	}
}

void ASLHorse::BeginOverlapPlayer()
{
	if (OverlapEnemyCount > 0)
	{
		return;
	}

	bShouldMove = true;
	bDeceleration = false;
}

void ASLHorse::EndOverlapPlayer()
{
	bDeceleration = true;
}

void ASLHorse::Accelerate(float DeltaTime)
{
	TargetSpeed = FMath::Clamp(TargetSpeed + IncreasePerSecond * DeltaTime, DefaultSpeed, MaxSpeed);
	CurrentSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaTime, InterpSpeed);
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}


void ASLHorse::Deceleration(float DeltaTime)
{
	TargetSpeed = FMath::Clamp(TargetSpeed - IncreasePerSecond * 4.0f * DeltaTime, DefaultSpeed, MaxSpeed);
	CurrentSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaTime, InterpSpeed);
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;

	if (CurrentSpeed <= DefaultSpeed)
	{
		bShouldMove = false;
	}
}




