// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/RunnerTypes.h"
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLRunnerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLRunnerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Runner|Montage")
    void PlayRunStart();

    // 성공 시(매칭 성공) 섹션으로 점프 (선택)
    UFUNCTION(BlueprintCallable, Category="Runner|Action")
    void PlayMatchedMontage(EHurdleState StateMatched, ERunnerMontageSection Section);

    // 실패 시(매칭 실패) 피격 몽타주 재생
    UFUNCTION(BlueprintCallable, Category="Runner|Action")
    void PlayHitMontage(EHurdleState FromObstacle, ERunnerMontageSection Section);

protected:
    void UnlockAction();
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    TObjectPtr<UAnimSequenceBase> RunStartSequence = nullptr; // In-Place

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    float RunStartBlendIn = 0.12f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    float RunStartBlendOut = 0.20f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    float RunStartPlayRate = 1.0f;
    
    // ==== 입력으로 들어오는 실행 요청 ====
    UPROPERTY(BlueprintReadOnly, Category="Runner|Action")
    ERunnerAction RequestedAction = ERunnerAction::None;

    // ==== 락(입력/중복 트리거 방지) ====
    UPROPERTY(BlueprintReadOnly, Category="Runner|Action")
    bool bActionLocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Action")
    float ActionLockDuration = 0.5f;

    // 점프/슬라이드: 전신(DefaultSlot), 공격: 상체(UpperBody)
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

    FTimerHandle NextAttackTimer;
    bool bAlreadyAttacked = false;
};