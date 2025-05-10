// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLAICharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
class ASLBaseCharacter;
/**
 * 
 */
UCLASS()
class STILLLOADING_API USLAICharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FName TagToCheck) const;
	
	UPROPERTY()
	TObjectPtr<ASLBaseCharacter> OwningCharacter;

	UPROPERTY()
	TObjectPtr<ASLBaseCharacter> TargetCharacter;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AnimData|LocomotionData")
	float Angle;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "FaceAO")
	float FacePitch;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "FaceAO")
	float FaceYaw;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool IsDead;
};
