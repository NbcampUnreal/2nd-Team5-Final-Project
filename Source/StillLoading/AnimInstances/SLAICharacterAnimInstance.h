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

	UFUNCTION(BlueprintCallable, Category = "Animation|Getters")
	bool GetIsAttacking();
	
protected:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FName TagToCheck) const;

	// --- Anim Data | References ---
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<ASLBaseCharacter> OwningCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<UCharacterMovementComponent> OwningMovementComponent; 

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<ASLBaseCharacter> TargetCharacter;

	// --- Anim Data | Movement ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	bool bHasAcceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float LocomotionDirection;

	
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

	// --- Anim Data | Combat Specific ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Combat Specific")
    EHitDirection HitDirectionVector;

	UFUNCTION()
	void AnimNotify_AttackEnd();
};
