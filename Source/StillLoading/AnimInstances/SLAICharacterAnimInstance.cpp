// Fill out your copyright notice in the Description page of Project Settings.


#include "SLAICharacterAnimInstance.h"

#include "AIController.h"
#include "KismetAnimationLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/SLAIBaseCharacter.h"
#include "Character/SLPlayerCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USLAICharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ASLPlayerCharacterBase>(TryGetPawnOwner());

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

bool USLAICharacterAnimInstance::DoesOwnerHaveTag(FName TagToCheck) const
{
	return TryGetPawnOwner()->ActorHasTag(TagToCheck);
}

void USLAICharacterAnimInstance::AnimNotify_AttackEnd()
{
	SetIsAttacking(false);
}

bool USLAICharacterAnimInstance::GetIsAttacking()
{
	return IsAttacking;
}