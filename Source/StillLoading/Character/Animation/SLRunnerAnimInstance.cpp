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
        if (!bAlreadyAttacked)
        {
            M = AttackMontage_1;
            bAlreadyAttacked = true;
            GetWorld()->GetTimerManager().SetTimer(NextAttackTimer,
                [this]
                {
                    if (bAlreadyAttacked)
                    {
                        bAlreadyAttacked = false;
                    }
                },
                0.6f,
                false);
        }
        else
        {
            M = AttackMontage_2;
            bAlreadyAttacked = false;
            GetWorld()->GetTimerManager().ClearTimer(NextAttackTimer);
        }
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

void USLRunnerAnimInstance::UnlockAction()
{
    bActionLocked   = false;
    RequestedAction = ERunnerAction::None;
}