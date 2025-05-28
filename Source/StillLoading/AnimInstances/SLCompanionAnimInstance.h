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

protected:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	ECompanionActionPattern GetCompanionPattern() const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCompanionPattern(const ECompanionActionPattern CompanionPattern);

	void UpdateSpeedComponents();
	void UpdateInputDirection();
	void UpdateAttackBlend(float DeltaTime);
	void UpdateAcceleratingBlend(float DeltaTime);
	
	// 공격 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat")
	float AttackLocomotionBlend;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat")
	bool bIsAttackCombo;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat")
	bool bIsMovableAttack;
    
	// 입력 방향 (AI의 경우 목표 방향)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float InputDirection_X;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float InputDirection_Y;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float InputDirectionDegree;
    
	// 이전 속도 기록 (부드러운 애니메이션 전환용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float LastSpeedDegree;
	
	// 속도 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float Speed_X;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float Speed_Y;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float SpeedDegree;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float SpeedLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Movement")
	float AcceleratingBlend;
	// 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|State")
	bool bIsRunning;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|State")
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|State")
	bool IsBattleMage;

	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	ECompanionActionPattern CompanionPattern;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|References", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ASLCompanionCharacter> OwningCompanionCharacter;
};