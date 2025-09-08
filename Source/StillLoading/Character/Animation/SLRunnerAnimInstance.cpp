// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Animation/SLRunnerAnimInstance.h"
#include "TimerManager.h"
#include "Animation/AnimMontage.h"

static const FName SLOT_UpperBody(TEXT("UpperBody"));

void USLRunnerAnimInstance::PlayRunStart()
{
	if (!RunStartSequence) return;

	PlaySlotAnimationAsDynamicMontage(
		RunStartSequence,
		FName("DefaultSlot"),
		RunStartBlendIn,
		RunStartBlendOut,
		RunStartPlayRate,
		1
	);
}

void USLRunnerAnimInstance::PlayMatchedMontage(EHurdleState StateMatched, ERunnerMontageSection Section)
{
	UAnimMontage* M = nullptr;

	switch (StateMatched)
	{
	case EHurdleState::Jump:
		M = JumpMontage;
		break;

	case EHurdleState::Sliding:
		M = SlideMontage;
		break;

	case EHurdleState::Attack:
	{
		// 시퀀스 중엔 공격 자체를 막아 블렌딩 문제 제거
		if (bIsPlayingSequence)
		{
			return;
		}

		const bool bPlayingA1 = (AttackMontage_1 && Montage_IsPlaying(AttackMontage_1));
		const bool bPlayingA2 = (AttackMontage_2 && Montage_IsPlaying(AttackMontage_2));

		// 규칙: Attack 재생 중 Attack 입력 → Attack2 우선
		if (bPlayingA1 && AttackMontage_2)
		{
			Montage_Stop(0.06f, AttackMontage_1); // 짧게 블렌드아웃
			M = AttackMontage_2;                  // 다음 컷으로 즉시 스왑
		}
		else if (bPlayingA2 && AttackMontage_2)
		{
			// Attack2 중 다시 입력이면 Attack2 리스타트(필요 시 A1로 토글해도 됨)
			Montage_Stop(0.06f, AttackMontage_2);
			M = AttackMontage_2;
		}
		else
		{
			M = (AttackMontage_1 ? AttackMontage_1 : AttackMontage_2);
		}

		checkf(M, TEXT("Attack montage is missing"));

		// UpperBody 슬롯 크로스페이드
		Montage_Play(M, 1.0f);
		if (Section != ERunnerMontageSection::None)
		{
			Montage_JumpToSection(GetMontageSectionName(Section), M);
		}
		return; // Attack 경로는 여기서 끝
	}

	default:
		break;
	}

	checkf(M, TEXT("No Montage"));

	// 전신(점프/슬라이드)은 새 입력이 오면 무조건 덮어씀(같은 컷이어도 재시작)
	Montage_Play(M, 1.0f);
	if (Section != ERunnerMontageSection::None)
	{
		Montage_JumpToSection(GetMontageSectionName(Section), M);
	}
}

void USLRunnerAnimInstance::PlayHitMontage(EHurdleState FromObstacle, ERunnerMontageSection Section)
{
	UAnimMontage* HitM = nullptr;

	switch (FromObstacle)
	{
	case EHurdleState::Jump:
		HitM = HitByJumpObstacleMontage;
		break;

	case EHurdleState::Sliding:
		HitM = HitBySlidingObstacleMontage;
		break;

	case EHurdleState::Attack:
		HitM = HitByAttackObstacleMontage;
		break;

	case EHurdleState::None:
		return;
	default:
		UE_LOG(LogTemp, Error, TEXT("PlayHitMontage: Invalid FromObstacle %d"), (int)FromObstacle);
		break;
	}
	
	checkf(HitM, TEXT("No Montage"));

	// UpperBody(공격 피격)면 슬롯 크로스페이드
	bool bIsUpper = false;
	for (const FSlotAnimationTrack& T : HitM->SlotAnimTracks)
	{
		if (T.SlotName == SLOT_UpperBody)
		{
			bIsUpper = true;
			break;
		}
	}

	if (bIsUpper)
	{
		Montage_Play(HitM, 1.0f);
		if (Section != ERunnerMontageSection::None)
		{
			Montage_JumpToSection(GetMontageSectionName(Section), HitM);
		}
		return;
	}

	// 전신 피격은 강하게 덮어쓰기
	StopAllMontages(0.05f);
	Montage_Play(HitM, 1.0f);
	if (Section != ERunnerMontageSection::None)
	{
		Montage_JumpToSection(GetMontageSectionName(Section), HitM);
	}

	// 짧은 입력 락(필요 시)
	bActionLocked = true;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			LockTimer, this, &USLRunnerAnimInstance::UnlockAction, 0.35f, false);
	}
}

void USLRunnerAnimInstance::UnlockAction()
{
	bActionLocked = false;
}
