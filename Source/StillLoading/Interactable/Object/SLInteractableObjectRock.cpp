
// Fill out your copyright notice in the Description page of Project Settings.

#include "SLInteractableObjectRock.h"
#include "StillLoading\Character\SLPlayerCharacter.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"

ASLInteractableObjectRock::ASLInteractableObjectRock()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // 기본적으로 Tick OFF
	TriggerType = ESLReactiveTriggerType::ERT_InteractKey;
}

void ASLInteractableObjectRock::PushInDirection(FVector Direction)
{
	// 이미 움직이고 있으면 무시
	if (bIsMoving) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Rock is already moving, ignoring push request"));
		return;
	}

	// 방향 벡터 정규화 및 유효성 검사
	Direction.Normalize();
	if (Direction.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid direction vector for rock push"));
		return;
	}

	MoveDirection = Direction;
	StartLocation = GetActorLocation();
	TargetLocation = StartLocation + MoveDirection * PushDistance;

	// 목표 위치의 Z값을 현재 위치와 동일하게 유지 (수평 이동만)
	TargetLocation.Z = StartLocation.Z;

	// 충돌 검사 - 목표 위치에 장애물이 있는지 확인
	FHitResult HitResult;
	FVector TraceStart = StartLocation;
	FVector TraceEnd = TargetLocation;
	
	// 바위의 충돌 반경을 고려한 Sphere Trace
	if (GetWorld()->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeSphere(RockRadius * 0.5f))) // 바위 반지름의 절반으로 여유 공간 확보
	{
		// 장애물이 있으면 충돌 지점까지만 이동
		float SafeDistance = FVector::Dist(StartLocation, HitResult.Location) - RockRadius * 0.6f;

		TargetLocation = StartLocation + MoveDirection * SafeDistance;
		TargetLocation.Z = StartLocation.Z;
	}

	bIsMoving = true;
	SetActorTickEnabled(true);  // Tick ON
	
	UE_LOG(LogTemp, Log, TEXT("Rock push started - From: %s To: %s"), 
		*StartLocation.ToString(), *TargetLocation.ToString());
}

void ASLInteractableObjectRock::MoveRock(float DeltaTime)
{
	if (!bIsMoving) return;

	FVector CurrentLocation = GetActorLocation();
	
	// 더 부드러운 보간을 위해 InterpSpeed 조정
	float InterpSpeed = FMath::Max(MoveSpeed, 0.1f); // 최소값 보장
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, InterpSpeed);

	// 이동 델타 계산
	FVector MoveDelta = NewLocation - CurrentLocation;

	// 회전 계산 (구르는 효과)
	if (!MoveDelta.IsNearlyZero(0.01f)) // 더 정밀한 임계값
	{
		float DistanceMoved = MoveDelta.Size();
		float RotationAngle = DistanceMoved / FMath::Max(RockRadius, 1.0f) * (180.0f / PI);
		
		// 회전축 계산 (이동 방향과 수직)
		FVector RotationAxis = FVector::CrossProduct(FVector::UpVector, MoveDirection).GetSafeNormal();
		
		if (!RotationAxis.IsNearlyZero())
		{
			FQuat DeltaRotation = FQuat(RotationAxis, FMath::DegreesToRadians(RotationAngle));
			FQuat CurrentRotation = GetActorQuat();
			SetActorRotation(DeltaRotation * CurrentRotation);
		}
	}

	// Z축 고정 및 위치 설정
	NewLocation.Z = StartLocation.Z;
	SetActorLocation(NewLocation);

	// 목표 지점 도달 검사 (더 정확한 거리 계산)
	float DistanceToTarget = FVector::Dist(NewLocation, TargetLocation);
	if (DistanceToTarget <= 5.0f) // 더 정밀한 도달 거리
	{
		// 정확한 목표 위치로 스냅
		FVector FinalLocation = TargetLocation;
		FinalLocation.Z = StartLocation.Z;
		SetActorLocation(FinalLocation);
		
		// 이동 완료
		bIsMoving = false;
		SetActorTickEnabled(false);
		
		UE_LOG(LogTemp, Log, TEXT("Rock movement completed at: %s"), *FinalLocation.ToString());
	}
}

void ASLInteractableObjectRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsMoving)
	{
		MoveRock(DeltaTime);
	}
}

void ASLInteractableObjectRock::OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType)
{
	// 캐릭터 유효성 검사
	if (!IsValid(InCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("InCharacter is invalid in OnInteracted!"));
		return;
	}

	// 캐릭터의 앞 방향으로 바위 밀기
	FVector PushDirection = InCharacter->GetActorForwardVector();
	PushInDirection(PushDirection);
}