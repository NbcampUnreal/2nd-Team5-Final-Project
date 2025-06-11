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

    CurrentActionPattern = OwningDoppelgangerCharacter->GetCurrentActionPattern();
    
    bIsGuardBroken = OwningDoppelgangerCharacter->IsGuardBroken();

    bIsDashing = (CurrentActionPattern == EDoppelgangerActionPattern::EDAP_Dash);
    
    if (bIsDashing)
    {
        CurrentDashDegree = OwningDoppelgangerCharacter->GetDashDegree();
    }
}

EDoppelgangerActionPattern USLDoppelgangerAnimInstance::GetCurrentPattern() const
{
    return CurrentActionPattern;
}

bool USLDoppelgangerAnimInstance::IsGuardBroken() const
{
    return bIsGuardBroken;
}

