// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCompanionAnimInstance.h"

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
}

void USLCompanionAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningCompanionCharacter = Cast<ASLCompanionCharacter>(OwningCharacter);
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