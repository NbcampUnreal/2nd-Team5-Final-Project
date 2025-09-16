// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/RunnerTypes.h"
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLRunnerAnimInstance.generated.h"

extern const FName SLOT_UpperBody;

UCLASS()
class STILLLOADING_API USLRunnerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 달리기 시작 컷(전신)
	UFUNCTION(BlueprintCallable, Category="Runner|Montage")
	void PlayRunStart();

	// 성공 행동(점프/슬라이드/어택)
	UFUNCTION(BlueprintCallable, Category="Runner|Action")
	void PlayMatchedMontage(EHurdleState StateMatched, ERunnerMontageSection Section);

	// 피격(점프/슬라이드/어택)
	UFUNCTION(BlueprintCallable, Category="Runner|Action")
	void PlayHitMontage(EHurdleState FromObstacle, ERunnerMontageSection Section);

protected:
	void UnlockAction();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Sequence")
	bool bIsPlayingSequence = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimSequenceBase> RunStartSequence = nullptr; // In-Place

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	float RunStartBlendIn = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	float RunStartBlendOut = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	float RunStartPlayRate = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category="Runner|Action")
	bool bActionLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Action")
	float ActionLockDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> JumpMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> SlideMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> AttackMontage_1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> AttackMontage_2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> HitByJumpObstacleMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> HitBySlidingObstacleMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> HitByAttackObstacleMontage = nullptr;

protected:
	FTimerHandle LockTimer;

	bool bAlreadyAttacked = false;
};
