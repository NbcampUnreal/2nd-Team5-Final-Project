// Fill out your copyright notice in the Description page of Project Settings.


#include "SLHomingProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"


ASLHomingProjectile::ASLHomingProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
    
	// 추적 설정
	if (ProjectileMovement)
	{
		ProjectileMovement->bIsHomingProjectile = false; // 수동으로 제어
		ProjectileMovement->ProjectileGravityScale = 0.0f;
	}
    
	// 추적 속성 설정
	HomingAccelerationMagnitude = 2000.0f;
	HomingDelay = 0.2f;
	bIsHoming = false;
}

void ASLHomingProjectile::BeginPlay()
{
	Super::BeginPlay();
    
	// 추적 시작 딜레이
	GetWorld()->GetTimerManager().SetTimer(
		HomingDelayTimer,
		this,
		&ASLHomingProjectile::StartHoming,
		HomingDelay,
		false
	);
}

void ASLHomingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	if (bIsHoming && HomingTarget.IsValid() && ProjectileMovement)
	{
		// 타겟 위치 (약간 위쪽을 노리도록)
		FVector TargetLocation = HomingTarget->GetActorLocation();
		TargetLocation.Z += 50.0f;
        
		// 현재 위치와 속도
		FVector CurrentLocation = GetActorLocation();
		FVector CurrentVelocity = ProjectileMovement->Velocity;
        
		// 타겟 방향 계산
		FVector DirectionToTarget = (TargetLocation - CurrentLocation).GetSafeNormal();
        
		// 부드러운 회전을 위한 속도 조정
		FVector DesiredVelocity = DirectionToTarget * CurrentVelocity.Size();
		FVector VelocityChange = (DesiredVelocity - CurrentVelocity).GetClampedToMaxSize(HomingAccelerationMagnitude * DeltaTime);
        
		// 새로운 속도 적용
		ProjectileMovement->Velocity = CurrentVelocity + VelocityChange;
        
		// 발사체 회전
		if (!ProjectileMovement->Velocity.IsZero())
		{
			SetActorRotation(ProjectileMovement->Velocity.Rotation());
		}
	}
}

void ASLHomingProjectile::SetHomingTarget(AActor* Target)
{
	HomingTarget = Target;
}

void ASLHomingProjectile::StartHoming()
{
	bIsHoming = true;
}

