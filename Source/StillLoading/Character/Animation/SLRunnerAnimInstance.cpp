// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/SLRunnerAnimInstance.h"
#include "TimerManager.h"

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

void USLRunnerAnimInstance::PushAction(ERunnerAction InAction)
{
    if (bActionLocked || InAction == ERunnerAction::None) return;

    // 전신 동작 중엔 상체 공격을 막고 싶다면(권장)
    if (InAction == ERunnerAction::Attack && IsFullBodyBusy())
        return;

    RequestedAction = InAction;
    bActionLocked   = true;

    PlayActionMontage(InAction);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            LockTimer,
            this, &USLRunnerAnimInstance::UnlockAction,
            ActionLockDuration, false);
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
        M = AttackMontage;
        break;
    default: break;
    }

    checkf(M, TEXT("No Montage"));

    if (!Montage_IsPlaying(M))
    {
        Montage_Play(M, 1.0f);
        Montage_JumpToSection(GetMontageSectionName(Section), M);
    }
}

void USLRunnerAnimInstance::PlayHitMontage(EHurdleState FromObstacle, ERunnerMontageSection Section)
{
    UAnimMontage* HitM = nullptr;
    switch (FromObstacle)
    {
    case EHurdleState::Jump:    HitM = HitByJumpObstacleMontage;    break;
    case EHurdleState::Sliding: HitM = HitBySlidingObstacleMontage; break;
    case EHurdleState::Attack:  HitM = HitByAttackObstacleMontage;  break;
    default: break;
    }

    checkf(HitM, TEXT("No Montage"));

    StopAllMontages(0.05f);  

    Montage_Play(HitM, 1.0f); 
    Montage_JumpToSection(GetMontageSectionName(Section), HitM);

    bActionLocked = true;
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            LockTimer,
            this,
            &USLRunnerAnimInstance::UnlockAction,
            0.35f,
            false);
    }
    
}

bool USLRunnerAnimInstance::IsFullBodyBusy() const
{
    return (JumpMontage  && Montage_IsPlaying(JumpMontage))
        || (SlideMontage && Montage_IsPlaying(SlideMontage));
}

void USLRunnerAnimInstance::UnlockAction()
{
    bActionLocked   = false;
    RequestedAction = ERunnerAction::None;
}