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
	UPROPERTY(BlueprintReadOnly, Category="Runner")
	ERunnerAction RequestedAction = ERunnerAction::None;

	UPROPERTY(BlueprintReadOnly, Category="Runner")
	bool bActionLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner")
	float ActionLockDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	UAnimMontage* JumpMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	UAnimMontage* SlideMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Montage")
	UAnimMontage* AttackMontage = nullptr;

	// 섹션 이름을 행동별로 분리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Sections")
	FName JumpMatchedSection   = FName("Jump_Matched");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Sections")
	FName SlidingMatchedSection= FName("Sliding_Matched");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Runner|Sections")
	FName AttackMatchedSection = FName("Attack_Matched");

	UFUNCTION(BlueprintCallable, Category="Runner")
	void PushAction(ERunnerAction InAction);

	UFUNCTION(BlueprintCallable, Category="Runner")
	void PlayMatchedMontage(EHurdleState StateMatched);

protected:
	FTimerHandle LockTimer;
	void UnlockAction();
	void PlayActionMontage(ERunnerAction InAction);
};