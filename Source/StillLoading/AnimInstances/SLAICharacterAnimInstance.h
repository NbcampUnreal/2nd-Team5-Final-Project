// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/SLAIBaseCharacter.h"
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

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetHitDirection(EHitDirection NewDirection);
    
	// 히트 상태 설정 함수
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsHit(bool bNewIsHit);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsAttacking(bool bNewIsAttacking);
	
protected:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FName TagToCheck) const;
	
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|LocomotionData")
	TObjectPtr<ASLBaseCharacter> OwningCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|LocomotionData")
	TObjectPtr<ASLBaseCharacter> TargetCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|LocomotionData")
	TObjectPtr<UCharacterMovementComponent> OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AnimData|LocomotionData")
	float Angle;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AnimData|FaceAO")
	float FacePitch;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AnimData|FaceAO")
	float FaceYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimData")
	bool IsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimData|FaceAO")
	bool ShouldLookAtPlayer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite , Category = "AnimData")
	bool IsDead;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dead")
	bool DeathAnimCompleted;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    bool bIsHit;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    EHitDirection HitDirectionVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float HitWeight;

	UFUNCTION()
	void AnimNotify_AttackEnd();
};
