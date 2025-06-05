// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCompanionAnimInstance.h"

#include "AI/Components/SLCompanionFlyingComponent.h"
#include "AI/GamePlayTag/AIGamePlayTag.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

USLCompanionAnimInstance::USLCompanionAnimInstance()
{
	AttackLocomotionBlend = 0.0f;
	bIsAttackCombo = false;
	bIsMovableAttack = false;
	LastSpeedDegree = 0.0f;
	Speed_X = 0.0f;
	Speed_Y = 0.0f;
	SpeedDegree = 0.0f;
	SpeedLength = 0.0f;
	bIsRunning = false;
	bIsAccelerating = false;

	VerticalVelocity = 0.0f;
	bIsAscending = false;
	bIsDescending = false;
	VerticalLeanAngle = 0.0f;
	PreviousLocation = FVector::ZeroVector;
	VerticalVelocityThreshold = 20.0f;
}

void USLCompanionAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		OwningCompanionCharacter = Cast<ASLCompanionCharacter>(OwningCharacter);
		PreviousLocation = OwningCharacter->GetActorLocation();
	}
}

void USLCompanionAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
	if (!OwningCompanionCharacter || !OwningMovementComponent)
	{
		return;
	}
	
	// 속도 컴포넌트 업데이트
	UpdateSpeedComponents();
	
	// 가속 상태 업데이트
	bIsAccelerating = bHasAcceleration;
	CompanionPattern = OwningCompanionCharacter->GetCurrentActionPattern();
	bIsTeleporting = OwningCompanionCharacter->GetIsTeleporting();
	UpdateAcceleratingBlend(DeltaSeconds);
	IsBattleMage = OwningCompanionCharacter->GetIsBattleMage();
	// 달리기 상태 업데이트 (속도 기반으로 판단, 임계값은 프로젝트에 맞게 조정)
	const float RunSpeedThreshold = 200.0f;
	bIsRunning = GroundSpeed > RunSpeedThreshold && !IsAttacking && !bIsHit;
	
	
	// 공격 블렌드 업데이트
	UpdateAttackBlend(DeltaSeconds);

	if (USLCompanionFlyingComponent* FlyingComponent = OwningCompanionCharacter->GetFlyingComponent())
	{
		bIsFlying = FlyingComponent->IsFlying();

		UE_LOG(LogTemp, Warning, TEXT("bIsFlying = FlyingComponent->IsFlying() : %d"), bIsFlying);
	}

	UpdateVerticalMovement(DeltaSeconds);
}

void USLCompanionAnimInstance::UpdateSpeedComponents()
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
	if (bIsAccelerating && SpeedLength > 200.0f)
	{
		LastSpeedDegree = SpeedDegree;
	}
}

void USLCompanionAnimInstance::UpdateAttackBlend(float DeltaTime)
{
	// 공격 블렌드는 SetMovableAttack 상태에 따라 업데이트
	float TargetBlend = bIsMovableAttack ? 1.0f : 0.0f;
	AttackLocomotionBlend = FMath::FInterpTo(AttackLocomotionBlend, TargetBlend, DeltaTime, 10.0f);
}

void USLCompanionAnimInstance::UpdateAcceleratingBlend(float DeltaTime)
{
	float TargetBlend = bIsAccelerating ? 1.0f : 0.0f;
	AcceleratingBlend = FMath::FInterpTo(AcceleratingBlend, TargetBlend, DeltaTime, 10.0f);
}

void USLCompanionAnimInstance::SetAttackCombo(bool bNewAttackCombo)
{
	bIsAttackCombo = bNewAttackCombo;
}

void USLCompanionAnimInstance::SetMovableAttack(bool bNewMovableAttack)
{
	bIsMovableAttack = bNewMovableAttack;
}

void USLCompanionAnimInstance::AnimNotify_PatternEnd()
{
	if (OwningCompanionCharacter)
	{
		// 캐릭터의 상태 직접 변경
		OwningCompanionCharacter->SetIsAttacking(false);
		OwningCompanionCharacter->SetCurrentActionPattern(ECompanionActionPattern::ECAP_None);
	}
    
}

ECompanionActionPattern USLCompanionAnimInstance::GetCompanionPattern() const
{
	return CompanionPattern;
}

void USLCompanionAnimInstance::SetCompanionPattern(const ECompanionActionPattern NewCompanionPattern)
{
	CompanionPattern = NewCompanionPattern;
}

void USLCompanionAnimInstance::UpdateVerticalMovement(float DeltaSeconds)
{
	if (!OwningCharacter || DeltaSeconds <= 0.0f)
	{
		return;
	}
    
	// 현재 위치 가져오기
	FVector CurrentLocation = OwningCharacter->GetActorLocation();
    
	// 위치 변화량으로 수직 속도 계산
	float DeltaZ = CurrentLocation.Z - PreviousLocation.Z;
	VerticalVelocity = DeltaZ / DeltaSeconds;
    
	// 상태 판단
	if (FMath::Abs(VerticalVelocity) < VerticalVelocityThreshold)
	{
		// 거의 움직이지 않음
		bIsAscending = false;
		bIsDescending = false;
	}
	else if (VerticalVelocity > VerticalVelocityThreshold)
	{
		// 상승 중
		bIsAscending = true;
		bIsDescending = false;
	}
	else if (VerticalVelocity < -VerticalVelocityThreshold)
	{
		// 하강 중
		bIsAscending = false;
		bIsDescending = true;
	}
    
	// 기울기 각도 계산
	float TargetLeanAngle = 0.0f;
	if (bIsAscending || bIsDescending)
	{
		// 속도에 비례한 기울기 (최대 ±30도)
		float NormalizedVelocity = FMath::Clamp(VerticalVelocity / 500.0f, -1.0f, 1.0f);
		TargetLeanAngle = NormalizedVelocity * 30.0f;
	}
	VerticalLeanAngle = FMath::FInterpTo(VerticalLeanAngle, TargetLeanAngle, DeltaSeconds, 4.0f);
    
	// 현재 위치 저장
	PreviousLocation = CurrentLocation;
    
	// 디버그 출력
	if (OwningCharacter->GetIsDebugMode())
	{
		UE_LOG(LogTemp, Warning, TEXT("Vertical Velocity: %.2f, Ascending: %s, Descending: %s"), 
			VerticalVelocity,
			bIsAscending ? TEXT("true") : TEXT("false"),
			bIsDescending ? TEXT("true") : TEXT("false"));
	}
}