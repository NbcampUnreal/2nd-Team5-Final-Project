// Fill out your copyright notice in the Description page of Project Settings.


#include "SLAICharacterAnimInstance.h"

#include "AIController.h"
#include "KismetAnimationLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AI/SLMonster/SLMonster.h"
#include "Components/SphereComponent.h"

void USLAICharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ASLAIBaseCharacter>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}

	bHasAcceleration = false;
	bIsHit = false;
	IsDead = false;
	IsDown = false;
	IsStun = false;
	IsAttacking = false;
	ShouldLookAtPlayer = false;
	bIsFalling = false;
	FallSpeed = 0.f;
	FaceYaw = 0.f;
	FacePitch = 0.f;
	Angle = 0.f;
}

void USLAICharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
    
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
	
    
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(),OwningCharacter->GetActorRotation());
							
	FVector Velocity = OwningCharacter->GetVelocity();
	FallSpeed = Velocity.Z;
    
	float DistanceToGround = GetDistanceToGround();
    
	if (DistanceToGround <= 200.0f && FallSpeed <= 0.0f)
	{
		bIsFalling = false;
	}
	else
	{
		bIsFalling = (FallSpeed < -100.0f) || (DistanceToGround > 200.0f);
	}
    
	Angle = UKismetMathLibrary::FindLookAtRotation(OwningCharacter->GetActorForwardVector(), OwningCharacter->GetVelocity()).Yaw;
    
	AAIController* AIController = Cast<AAIController>(OwningCharacter->GetController());
	if (AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (BlackboardComp)
		{  
			if (BlackboardComp->GetValueAsObject(FName("TargetActor"))) 
			{
				TargetCharacter = Cast<ASLPlayerCharacterBase>(BlackboardComp->GetValueAsObject(FName("TargetActor")));
			}
		}
	}

	if (TargetCharacter)
	{
		FVector DirectionUnitVector = UKismetMathLibrary::GetDirectionUnitVector(GetOwningActor()->GetActorLocation(),TargetCharacter->GetActorLocation());
		FRotator NormalizeRotator = UKismetMathLibrary::NormalizedDeltaRotator(GetOwningActor()->GetActorRotation(),DirectionUnitVector.ToOrientationRotator());
		FaceYaw = NormalizeRotator.GetInverse().Yaw;
		FacePitch = NormalizeRotator.GetInverse().Pitch;
	}
}

void USLAICharacterAnimInstance::SetHitDirection(EHitDirection NewDirection)
{
	HitDirectionVector = NewDirection;
}

void USLAICharacterAnimInstance::SetIsHit(bool bNewIsHit)
{
	bIsHit = bNewIsHit;
}

void USLAICharacterAnimInstance::SetIsDead(bool bNewIsDead)
{
	IsDead = bNewIsDead;
}

void USLAICharacterAnimInstance::SetIsDown(bool bNewIsDown)
{
	IsDown = bNewIsDown;
}

void USLAICharacterAnimInstance::SetIsStun(bool bNewIsStun)
{
	IsStun = bNewIsStun;
}

void USLAICharacterAnimInstance::SetIsAttacking(bool bNewIsAttacking)
{
	IsAttacking = bNewIsAttacking;
}

void USLAICharacterAnimInstance::SetShouldLookAtPlayer(bool bNewShouldLookAtPlayer)
{
	ShouldLookAtPlayer = bNewShouldLookAtPlayer;
}

bool USLAICharacterAnimInstance::IsTargetBehindCharacter(float AngleThreshold) const
{
	if (!OwningCharacter || !TargetCharacter)
	{
		return false;
	}
    
	FVector CharacterForward = OwningCharacter->GetActorForwardVector();
    
	FVector DirectionToTarget = (TargetCharacter->GetActorLocation() - OwningCharacter->GetActorLocation()).GetSafeNormal();
    
	float DotProduct = FVector::DotProduct(CharacterForward, DirectionToTarget);
    
	// 각도 임계값을 내적 임계값으로 변환
	float DotThreshold = FMath::Cos(FMath::DegreesToRadians(AngleThreshold * 0.5f));
    
	// 내적이 음수이면 타겟이 캐릭터 뒤에 있음
	return DotProduct < -DotThreshold;
}

bool USLAICharacterAnimInstance::DoesOwnerHaveTag(FName TagToCheck) const
{
	return TryGetPawnOwner()->ActorHasTag(TagToCheck);
}

void USLAICharacterAnimInstance::AnimNotify_AttackEnd()
{
	APawn* Pawn = TryGetPawnOwner();
	ASLMonster* Monster = Cast<ASLMonster>(Pawn);
	if (Monster)
	{
		SetIsAttacking(false);
		/*Monster->AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
		TWeakObjectPtr<ASLMonster> WeakMonster = Monster;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[WeakMonster]()
		{
			if (WeakMonster.IsValid() && WeakMonster->AttackSphere)
			{
				WeakMonster->AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		},
			5.f, // 지연 시간
			false
		);
	}
}

bool USLAICharacterAnimInstance::GetIsAttacking()
{
	return IsAttacking;
}

bool USLAICharacterAnimInstance::GetbIsInCombat()
{
	return bIsInCombat;
}

float USLAICharacterAnimInstance::GetDistanceToGround() const
{
	if (!OwningCharacter)
	{
		return 0.0f;
	}

	// 캐릭터의 현재 위치
	FVector StartLocation = OwningCharacter->GetActorLocation();
    
	// 아래쪽으로 레이캐스트할 끝점
	FVector EndLocation = StartLocation + FVector(0, 0, -5000.0f);
    
	// 레이캐스트 설정
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwningCharacter);
	QueryParams.bTraceComplex = false;
    
	// 레이캐스트 실행
	if (OwningCharacter->GetWorld()->LineTraceSingleByChannel(
		HitResult, StartLocation, EndLocation, ECC_WorldStatic, QueryParams))
	{
		return FVector::Dist(StartLocation, HitResult.Location);
	}
    
	// 히트되지 않았으면 큰 값 반환
	return 5000.0f;
}
