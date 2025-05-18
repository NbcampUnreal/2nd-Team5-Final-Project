// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigame/Object/SLReactiveObjectRock.h"
#include "StillLoading\Character\SLPlayerCharacter.h"

ASLReactiveObjectRock::ASLReactiveObjectRock()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // 기본적으로 Tick OFF
}

void ASLReactiveObjectRock::PushInDirection(FVector Direction)
{
	if (bIsMoving) return;

	MoveDirection = Direction.GetSafeNormal();
	StartLocation = GetActorLocation();
	TargetLocation = StartLocation + MoveDirection * PushDistance;

	bIsMoving = true;
	SetActorTickEnabled(true);  // Tick ON
}

void ASLReactiveObjectRock::MoveRock(float DeltaTime)
{
	if (!bIsMoving) return;

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);

	FVector MoveDelta = NewLocation - CurrentLocation;

	if (!MoveDelta.IsNearlyZero())
	{
		float DistanceMoved = MoveDelta.Size();
		float RotationAngle = DistanceMoved / RockRadius * (180.0f / PI);
		FVector RotationAxis = FVector::CrossProduct(FVector::UpVector, MoveDirection);
		FQuat DeltaRotation = FQuat(RotationAxis, FMath::DegreesToRadians(RotationAngle));
		SetActorRotation(DeltaRotation * GetActorQuat());
	}

	SetActorLocation(NewLocation);

	if (FVector::DistSquared(NewLocation, TargetLocation) <= 1.0f)
	{
		SetActorLocation(TargetLocation);
		bIsMoving = false;
		SetActorTickEnabled(false);
	}
}

void ASLReactiveObjectRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsMoving)
	{
		MoveRock(DeltaTime);
		UE_LOG(LogTemp, Warning, TEXT("MoveRock"));
	}
	UE_LOG(LogTemp, Warning, TEXT("Tick"));
}

void ASLReactiveObjectRock::OnReacted(const ASLPlayerCharacter* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	checkf(IsValid(InCharacter), TEXT("InCharacter is nullptr!"));

	PushInDirection(FVector::ForwardVector);
	UE_LOG(LogTemp, Warning, TEXT("PushInDirection"));
	/*FVector RockLocation = GetActorLocation();

	FVector HitOrigin = InCharacter->GetActorLocation();
	FVector KnockbackDirection = (RockLocation - HitOrigin).GetSafeNormal();


	UPrimitiveComponent* Primitive = FindComponentByClass<UPrimitiveComponent>();
	if (Primitive && Primitive->IsSimulatingPhysics())
	{
		StaticMeshComp->SetSimulatePhysics(true);
		Primitive->AddImpulse(KnockbackDirection * KnockbackForce, NAME_None, true);
	}*/
}
