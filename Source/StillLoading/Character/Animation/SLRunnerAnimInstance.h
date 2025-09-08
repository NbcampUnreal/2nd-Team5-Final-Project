// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/RunnerTypes.h"
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLRunnerAnimInstance.generated.h"

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
	// ===== 시퀀서 상태(네가 사용 중인 bIsPlayingSequence 유지) =====
	// 시퀀스 진행 중엔 Attack 실행을 차단하기 위해 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Sequence")
	bool bIsPlayingSequence = false;

	// ===== RunStart =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimSequenceBase> RunStartSequence = nullptr; // In-Place

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	float RunStartBlendIn = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	float RunStartBlendOut = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	float RunStartPlayRate = 1.0f;

	// ===== 락(입력/중복 트리거 방지) =====
	UPROPERTY(BlueprintReadOnly, Category="Runner|Action")
	bool bActionLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Action")
	float ActionLockDuration = 0.5f;

	// ===== 전신(점프/슬라이드), 상체/전신(공격) =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> JumpMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> SlideMontage = nullptr;

	// 공격 체인(UpperBody 슬롯 권장)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> AttackMontage_1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> AttackMontage_2 = nullptr;

	// ===== 피격 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> HitByJumpObstacleMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> HitBySlidingObstacleMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	TObjectPtr<UAnimMontage> HitByAttackObstacleMontage = nullptr;

protected:
	FTimerHandle LockTimer;

	// 과거 토글용 변수/타이머(필요 시 사용), 현재 로직에선 강제 스왑으로 처리
	FTimerHandle NextAttackTimer;
	bool bAlreadyAttacked = false;
};
