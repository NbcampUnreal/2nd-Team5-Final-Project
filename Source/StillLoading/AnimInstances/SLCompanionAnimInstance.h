// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLAICharacterAnimInstance.h"
#include "AI/Companion/SLCompanionCharacter.h"
#include "SLCompanionAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLCompanionAnimInstance : public USLAICharacterAnimInstance
{
	GENERATED_BODY()

public:
	USLCompanionAnimInstance();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
    
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetAttackCombo(bool bNewAttackCombo);
    
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetMovableAttack(bool bNewMovableAttack);

	UFUNCTION()
	void AnimNotify_PatternEnd();
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	ECompanionActionPattern GetCompanionPattern() const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCompanionPattern(const ECompanionActionPattern CompanionPattern);
	
	UFUNCTION()
	void UpdateAttackBlend(float DeltaTime);
	
	UFUNCTION()
	void UpdateAcceleratingBlend(float DeltaTime);

	UFUNCTION()
	void UpdateVerticalMovement(float DeltaSeconds);
	
	// 공격 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat")
	float AttackLocomotionBlend;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat")
	bool bIsAttackCombo;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat")
	bool bIsMovableAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float AcceleratingBlend;
	// 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|State")
	bool bIsRunning;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|State")
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|State")
	bool IsBattleMage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|State")
	bool bIsTeleporting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|State")
	bool bIsFlying;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|GameplayTags")
	FGameplayTagContainer ActiveTags;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float VerticalVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	bool bIsAscending;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	bool bIsDescending;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float VerticalLeanAngle;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	ECompanionActionPattern CompanionPattern;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|References", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ASLCompanionCharacter> OwningCompanionCharacter;

	UPROPERTY()
	FVector PreviousLocation;

	UPROPERTY()
	float VerticalVelocityThreshold;
};