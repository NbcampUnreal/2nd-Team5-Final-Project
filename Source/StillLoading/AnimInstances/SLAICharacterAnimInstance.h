// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/SLAIBaseCharacter.h"
#include "SLAICharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
class ASLPlayerCharacterBase;
/**
 * 
 */
UCLASS()
class STILLLOADING_API USLAICharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USLAICharacterAnimInstance();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Animation|Setters")
	void SetHitDirection(EHitDirection NewDirection);
    
	UFUNCTION(BlueprintCallable, Category = "Animation|Setters")
	void SetIsHit(bool bNewIsHit);

	UFUNCTION(BlueprintCallable, Category = "Animation|Setters")
	void SetIsDead(bool bNewIsDead);

	UFUNCTION(BlueprintCallable, Category = "Animation|Setters")
	void SetIsDown(bool bNewIsDown);

	UFUNCTION(BlueprintCallable, Category = "Animation|Setters")
	void SetIsStun(bool bNewIsStun);

	UFUNCTION(BlueprintCallable, Category = "Animation|Setters")
	void SetIsAttacking(bool bNewIsAttacking);

	UFUNCTION(BlueprintCallable, Category = "Animation|Setters")
	void SetShouldLookAtPlayer(bool bNewShouldLookAtPlayer);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsTargetBehindCharacter(float AngleThreshold) const;

	UFUNCTION(BlueprintCallable, Category = "Animation|Getters")
	bool GetIsAttacking();

	UFUNCTION(BlueprintCallable, Category = "Animation|Getters")
	bool GetbIsInCombat();

	UFUNCTION(BlueprintCallable, Category = "Animation|Setters")
	void SetDamagePosition(const FVector& NewDamagePosition);
	
	UFUNCTION(BlueprintCallable, Category = "Animation|Getters")
	FVector GetDamagePosition() const;
protected:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FName TagToCheck) const;

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetDistanceToGround() const;
	
	// --- Anim Data | References ---
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<ASLAIBaseCharacter> OwningCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<UCharacterMovementComponent> OwningMovementComponent; 

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<ACharacter> TargetCharacter;

	// --- Anim Data | Movement ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	bool bHasAcceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float LocomotionDirection;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|Movement")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|Movement")
	float FallSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|Movement")
	bool bIsJump;
	// --- Anim Data | Aiming & Looking ---
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimData|Movement")	// 수정 예정
	float Angle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AnimData|Aiming") 
	bool ShouldLookAtPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimData|Aiming") 
	float FacePitch; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimData|Aiming")
	float FaceYaw; 

	// --- Anim Data | State ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|State")
	bool IsAttacking = false; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|State") 
    bool bIsHit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|State") 
	bool IsDown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|State") 
	bool IsStun;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|State")
	bool IsDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|State")
	bool IsExecution;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|State")
	bool bIsInCombat;
	
	// --- Anim Data | Combat Specific ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Combat Specific")
    EHitDirection HitDirectionVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat Specific")
	FVector DamagePosition;

private:
	FVector PreviousVelocity;
	float PreviousGroundSpeed;
};
