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

    // 실행 요청(성공 동작 재생 + 락)
    UFUNCTION(BlueprintCallable, Category="Runner|Action")
    void PushAction(ERunnerAction InAction);

    // 성공 시(매칭 성공) 섹션으로 점프 (선택)
    UFUNCTION(BlueprintCallable, Category="Runner|Action")
    void PlayMatchedMontage(EHurdleState StateMatched, ERunnerMontageSection Section);

    // 실패 시(매칭 실패) 피격 몽타주 재생
    UFUNCTION(BlueprintCallable, Category="Runner|Action")
    void PlayHitMontage(EHurdleState FromObstacle, ERunnerMontageSection Section);

protected:
    void PlayActionMontage(ERunnerAction InAction);
    void UnlockAction();

    // 전신 동작(점프/슬라이드) 진행 중인지 → 상체 공격 금지 등에서 사용
    bool IsFullBodyBusy() const;

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
    UAnimMontage* JumpMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    UAnimMontage* SlideMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    UAnimMontage* AttackMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    UAnimMontage* HitByJumpObstacleMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    UAnimMontage* HitBySlidingObstacleMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
    UAnimMontage* HitByAttackObstacleMontage = nullptr;

protected:
    FTimerHandle LockTimer;
};