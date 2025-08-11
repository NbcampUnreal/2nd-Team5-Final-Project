// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/SLRunnerAnimInstance.h"

void USLRunnerAnimInstance::PushAction(ERunnerAction InAction)
{
	if (bActionLocked || InAction == ERunnerAction::None) return;

	RequestedAction = InAction;
	bActionLocked   = true;

	PlayActionMontage(InAction);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			LockTimer, this, &USLRunnerAnimInstance::UnlockAction, ActionLockDuration, false);
	}
}

void USLRunnerAnimInstance::PlayActionMontage(ERunnerAction InAction)
{
	UAnimMontage* M = nullptr;
	switch (InAction)
	{
	case ERunnerAction::Jump:   M = JumpMontage;   break;
	case ERunnerAction::Slide:  M = SlideMontage;  break;
	case ERunnerAction::Attack: M = AttackMontage; break;
	default: break;
	}
	if (M) Montage_Play(M, 1.0f);
}

void USLRunnerAnimInstance::PlayMatchedMontage(EHurdleState StateMatched)
{
	UAnimMontage* M = nullptr;
	FName Section   = NAME_None;

	switch (StateMatched)
	{
	case EHurdleState::Jump:
		M = JumpMontage;   Section = JumpMatchedSection;    break;
	case EHurdleState::Sliding:
		M = SlideMontage;  Section = SlidingMatchedSection; break;
	case EHurdleState::Attack:
		M = AttackMontage; Section = AttackMatchedSection;  break;
	default: break;
	}

	if (M)
	{
		if (!Montage_IsPlaying(M)) Montage_Play(M, 1.0f);
		if (Section != NAME_None)  Montage_JumpToSection(Section, M);
	}
}

void USLRunnerAnimInstance::UnlockAction()
{
	bActionLocked   = false;
	RequestedAction = ERunnerAction::None;
}
