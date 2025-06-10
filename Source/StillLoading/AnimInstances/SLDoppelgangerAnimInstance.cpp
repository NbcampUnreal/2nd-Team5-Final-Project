// Fill out your copyright notice in the Description page of Project Settings.

#include "SLDoppelgangerAnimInstance.h"

USLDoppelgangerAnimInstance::USLDoppelgangerAnimInstance()
{
    OwningDoppelgangerCharacter = nullptr;
    CurrentActionPattern = EDoppelgangerActionPattern::EDAP_None;
    bIsGuardBroken = false;
}

void USLDoppelgangerAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwningDoppelgangerCharacter = Cast<ASLDoppelgangerCharacter>(OwningCharacter);
}

void USLDoppelgangerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

    if (!OwningDoppelgangerCharacter)
    {
        return;
    }

    // 도플갱어의 현재 액션 패턴 업데이트
    CurrentActionPattern = OwningDoppelgangerCharacter->GetCurrentActionPattern();
    
    bIsGuardBroken = OwningDoppelgangerCharacter->IsGuardBroken();
}

EDoppelgangerActionPattern USLDoppelgangerAnimInstance::GetCurrentPattern() const
{
    return CurrentActionPattern;
}

bool USLDoppelgangerAnimInstance::IsGuardBroken() const
{
    return bIsGuardBroken;
}

