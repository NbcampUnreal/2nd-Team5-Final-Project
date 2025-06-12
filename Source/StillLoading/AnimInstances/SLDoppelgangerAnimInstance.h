// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLAICharacterAnimInstance.h"
#include "AI/Doppelganger/SLDoppelgangerCharacter.h"
#include "SLDoppelgangerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLDoppelgangerAnimInstance : public USLAICharacterAnimInstance
{
    GENERATED_BODY()

public:
    USLDoppelgangerAnimInstance();
    virtual void NativeInitializeAnimation() override;
    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "Animation")
    EDoppelgangerActionPattern GetCurrentPattern() const;

    UFUNCTION(BlueprintCallable, Category = "Animation")
    bool IsGuardBroken() const;

protected:

private:
    
    UPROPERTY(BlueprintReadOnly, Category = "AnimData|References", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ASLDoppelgangerCharacter> OwningDoppelgangerCharacter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat", meta = (AllowPrivateAccess = "true"))
    EDoppelgangerActionPattern CurrentActionPattern;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Combat", meta = (AllowPrivateAccess = "true"))
    bool bIsGuardBroken;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Dash", meta = (AllowPrivateAccess = "true"))
    float CurrentDashDegree;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Dash", meta = (AllowPrivateAccess = "true"))
    bool bIsDashing;
};