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
    
	HitDirectionVector = EHitDirection::EHD_Back; // enum 기본값에 맞게 조정
	DamagePosition =  FVector::ZeroVector; 
	// 이전 프레임 데이터 초기화
	PreviousVelocity = FVector::ZeroVector;
	PreviousGroundSpeed = 0.0f;
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
	// 현재 속도 계산
	FVector CurrentVelocity = OwningCharacter->GetVelocity();
	GroundSpeed = CurrentVelocity.Size2D();
    
	// AI 캐릭터를 위한 가속도 감지
	const float MovementThreshold = 50.0f; // 이동으로 간주할 최소 속도
	const float AccelerationThreshold = 15.0f; // 가속으로 간주할 속도 변화량 (AI용으로 낮춤)
	const float StopThreshold = 5.0f; // 정지로 간주할 속도
	
	// 기존 방식
	/*FVector CurrentAcceleration = OwningMovementComponent->GetCurrentAcceleration();
	bool bHasEngineAcceleration = CurrentAcceleration.SizeSquared2D() > 0.f;*/
	
	// AI용 방식: 속도 변화 감지
	float SpeedDifference = FMath::Abs(GroundSpeed - PreviousGroundSpeed);
	bool bHasSpeedChange = SpeedDifference > AccelerationThreshold;
	
	// 가속 시작/정지 감지
	bool bStartingMovement = (PreviousGroundSpeed <= StopThreshold && GroundSpeed > MovementThreshold);
	bool bStoppingMovement = (PreviousGroundSpeed > MovementThreshold && GroundSpeed <= StopThreshold);
	
	// 방향 변화 감지 (회전하면서 이동하는 경우)
	bool bHasDirectionChange = false;
	if (GroundSpeed > MovementThreshold && PreviousGroundSpeed > MovementThreshold)
	{
		FVector CurrentDirection = CurrentVelocity.GetSafeNormal2D();
		FVector PreviousDirection = PreviousVelocity.GetSafeNormal2D();
		float DirectionDot = FVector::DotProduct(CurrentDirection, PreviousDirection);
		bHasDirectionChange = DirectionDot < 0.9f; // 약 25도 이상 방향 변화
	}
	
	// 최종 가속도 상태 결정
	bHasAcceleration = bHasSpeedChange || bStartingMovement || bStoppingMovement || bHasDirectionChange;
	
	// 다음 프레임을 위해 이전 값들 저장
	PreviousVelocity = CurrentVelocity;
	PreviousGroundSpeed = GroundSpeed;
    
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(CurrentVelocity, OwningCharacter->GetActorRotation());
							
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
}

/*void USLAICharacterAnimInstance::SetHitDirection(EHitDirection NewDirection)
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
}*/

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
