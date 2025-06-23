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

USLAICharacterAnimInstance::USLAICharacterAnimInstance()
{
	OwningCharacter = nullptr;
	OwningMovementComponent = nullptr;
	TargetCharacter = nullptr;
    
	GroundSpeed = 0.0f;
	bHasAcceleration = false;
	LocomotionDirection = 0.0f;
	bIsFalling = false;
	FallSpeed = 0.0f;
	bIsJump = false;
    
	Angle = 0.0f;
	ShouldLookAtPlayer = false;
	FacePitch = 0.0f;
	FaceYaw = 0.0f;
    
	IsAttacking = false;
	bIsHit = false;
	IsDown = false;
	IsStun = false;
	IsDead = false;
	IsExecution = false;
	bIsInCombat = false;
	bIsAirHit = false;
	
	HitDirectionVector = EHitDirection::EHD_Back; // enum 기본값에 맞게 조정
	DamagePosition =  FVector::ZeroVector; 
	// 이전 프레임 데이터 초기화
	PreviousVelocity = FVector::ZeroVector;
	PreviousGroundSpeed = 0.0f;
	bOrientRotationToMovement = true;
}

void USLAICharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ASLAIBaseCharacter>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();

		// 초기 속도값 설정
		PreviousVelocity = OwningCharacter->GetVelocity();
		PreviousGroundSpeed = PreviousVelocity.Size2D();
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
	bIsAirHit = false;
}

void USLAICharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	bIsHit = OwningCharacter->GetIsHit();
	IsDown = OwningCharacter->GetIsDown();
	IsStun = OwningCharacter->GetIsStun();
	IsAttacking = OwningCharacter->GetIsAttacking();
	IsDead = OwningCharacter->GetIsDead();
	HitDirectionVector = OwningCharacter->GetHitDirection();
	ShouldLookAtPlayer = OwningCharacter->GetShouldLookAtPlayer();
	DamagePosition = OwningCharacter->GetHitDirectionVector();
	bIsJump = OwningCharacter->IsJumping();
	IsLoop = OwningCharacter->GetIsLoop();
	bIsAirHit = OwningCharacter->GetIsAirHit();
	
	// 현재 속도 계산
	FVector CurrentVelocity = OwningCharacter->GetMovementComponent()->GetVelocityForNavMovement();
	GroundSpeed = CurrentVelocity.Size2D();
    
	// AI 캐릭터를 위한 가속도 감지
	const float MovementThreshold = 50.0f;
	const float AccelerationThreshold = 15.0f;
	const float StopThreshold = 5.0f;
	
	float SpeedDifference = FMath::Abs(GroundSpeed - PreviousGroundSpeed);
	bool bHasSpeedChange = SpeedDifference > AccelerationThreshold;
	
	bool bStartingMovement = (PreviousGroundSpeed <= StopThreshold && GroundSpeed > MovementThreshold);
	bool bStoppingMovement = (PreviousGroundSpeed > MovementThreshold && GroundSpeed <= StopThreshold);
	
	bool bHasDirectionChange = false;
	if (GroundSpeed > MovementThreshold && PreviousGroundSpeed > MovementThreshold)
	{
		FVector CurrentDirection = CurrentVelocity.GetSafeNormal2D();
		FVector PreviousDirection = PreviousVelocity.GetSafeNormal2D();
		float DirectionDot = FVector::DotProduct(CurrentDirection, PreviousDirection);
		bHasDirectionChange = DirectionDot < 0.9f;
	}
	
	bHasAcceleration = bHasSpeedChange || bStartingMovement || bStoppingMovement || bHasDirectionChange;
	
	PreviousVelocity = CurrentVelocity;
	PreviousGroundSpeed = GroundSpeed;
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(CurrentVelocity, OwningCharacter->GetActorRotation());
	if (OwningMovementComponent)
	{
		bOrientRotationToMovement = OwningMovementComponent->bOrientRotationToMovement;
	}
	
	FallSpeed = CurrentVelocity.Z;
    
	float DistanceToGround = GetDistanceToGround();
    
	if (DistanceToGround <= 200.0f && FallSpeed <= 0.0f)
	{
		bIsFalling = false;
	}
	else
	{
		bIsFalling = (FallSpeed < -100.0f) || (DistanceToGround > 200.0f);
	}
    
	Angle = UKismetMathLibrary::FindLookAtRotation(OwningCharacter->GetActorForwardVector(), CurrentVelocity).Yaw;
    
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
	
	UpdateSpeedComponents();
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

bool USLAICharacterAnimInstance::GetIsAttacking()
{
	return IsAttacking;
}

bool USLAICharacterAnimInstance::GetbIsInCombat()
{
	return bIsInCombat;
}

void USLAICharacterAnimInstance::SetDamagePosition(const FVector& NewDamagePosition)
{
	DamagePosition = NewDamagePosition;
}

FVector USLAICharacterAnimInstance::GetDamagePosition() const
{
	return DamagePosition;
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

void USLAICharacterAnimInstance::UpdateSpeedComponents()
{
	if (!OwningCharacter)
	{
		return;
	}
	
	FVector Velocity = OwningCharacter->GetVelocity();
	FVector ForwardVector = OwningCharacter->GetActorForwardVector();
	FVector RightVector = OwningCharacter->GetActorRightVector();
	
	// 속도를 캐릭터의 로컬 좌표계로 변환
	Speed_X = FVector::DotProduct(Velocity, ForwardVector);
	Speed_Y = FVector::DotProduct(Velocity, RightVector);
	
	// 속도 각도 계산
	SpeedDegree = UKismetMathLibrary::DegAtan2(Speed_Y, Speed_X);
	
	// 속도 크기 계산 (보간 적용)
	float TargetSpeedLength = Velocity.Size();
	SpeedLength = FMath::FInterpTo(SpeedLength, TargetSpeedLength, GetWorld()->GetDeltaSeconds(), 20.0f);
	
	// 이동 중이고 가속 중일 때만 마지막 속도 각도 업데이트
	if (bHasAcceleration && SpeedLength > 200.0f)
	{
		LastSpeedDegree = SpeedDegree;
	}
}