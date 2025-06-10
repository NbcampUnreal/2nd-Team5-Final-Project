// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SLAIBaseCharacter.h"
#include "SLDoppelgangerCharacter.generated.h"

class ASLItem;

UENUM(BlueprintType)
enum class EDoppelgangerActionPattern : uint8
{
    EDAP_None                           UMETA(DisplayName = "None"),
    
    // 콤보 공격
    EDAP_Combo_Attack_01                UMETA(DisplayName = "DG Combo Attack 01"),
    EDAP_Combo_Attack_02                UMETA(DisplayName = "DG Combo Attack 02"),
    EDAP_Combo_Attack_03                UMETA(DisplayName = "DG Combo Attack 03"),
    EDAP_Combo_Attack_04                UMETA(DisplayName = "DG Combo Attack 04"),
    EDAP_Combo_Attack_Air               UMETA(DisplayName = "DG Combo Attack Air"),
    
    // 특수 공격
    EDAP_Attack_Up_01                   UMETA(DisplayName = "DG Attack Up 01"),
    EDAP_Attack_Up_Floor_To_Air_02      UMETA(DisplayName = "DG Attack Up Floor To Air 02"),
    EDAP_Run_Attack_02                  UMETA(DisplayName = "DG Run Attack 02"),
    EDAP_Dash_Air_Attack                UMETA(DisplayName = "DG Dash Air Attack"),
    
    // 방어 및 지원
    EDAP_Guard                          UMETA(DisplayName = "DG Guard"),
    EDAP_Parry                          UMETA(DisplayName = "DG Parry"),
    EDAP_Buff_Cast                      UMETA(DisplayName = "DG Buff Cast"),
    
    // 가드 파괴
    EDAP_Guard_Break                    UMETA(DisplayName = "DG Guard Break")
};

UENUM(BlueprintType)
enum class EDoppelgangerGuardState : uint8
{
    EDGS_None                           UMETA(DisplayName = "None"),
    EDGS_Guarding                       UMETA(DisplayName = "Guarding"),
    EDGS_GuardBroken                    UMETA(DisplayName = "Guard Broken")
};

UCLASS()
class STILLLOADING_API ASLDoppelgangerCharacter : public ASLAIBaseCharacter
{
    GENERATED_BODY()

public:
    ASLDoppelgangerCharacter();
    virtual void Tick(float DeltaTime) override;
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    EDoppelgangerActionPattern GetCurrentActionPattern() const;
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetCurrentActionPattern(EDoppelgangerActionPattern NewPattern);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    EDoppelgangerActionPattern SelectRandomPattern();
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    EDoppelgangerActionPattern SelectRandomPatternFromTags(const FGameplayTagContainer& PatternTags);
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    FGameplayTagContainer GetComboAttackPatterns() const;
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    FGameplayTagContainer GetSpecialAttackPatterns() const;
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    FGameplayTagContainer GetDefensePatterns() const;
    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    FGameplayTagContainer GetAllAttackPatterns() const;

    UFUNCTION(BlueprintCallable, Category = "Combat|Guard")
    void OnGuardSuccess();

    UFUNCTION(BlueprintCallable, Category = "Combat|Guard")
    void TriggerGuardBreak();

    UFUNCTION(BlueprintCallable, Category = "Combat|Guard")
    void RecoverFromGuardBreak();

    UFUNCTION(BlueprintCallable, Category = "Combat|Guard")
    void ResetGuardCounter();

    UFUNCTION(BlueprintCallable, Category = "Combat|Guard")
    bool IsGuardBroken() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<ASLItem> SwordClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<ASLItem> ShieldClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<ASLItem> Sword;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<ASLItem> Shield;

protected:
    virtual void BeginPlay() override;
    virtual void CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType) override;

    void InitializePatternMapping();
    EDoppelgangerActionPattern GetActionPatternForTag(const FGameplayTag& Tag) const;
    void AttachItemToHand(AActor* ItemActor, const FName SocketName) const;
    void StartGuardCounterResetTimer();
    void StopGuardCounterResetTimer();
    void ResetGuardCounterWithDelay();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Guard")
    int32 MaxGuardCount;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Guard")
    float GuardBreakDuration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Guard")
    float GuardCounterResetDelay;

private:
    EDoppelgangerActionPattern CurrentActionPattern;
    int32 MaxRecentPatternMemory;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    FGameplayTagContainer RecentlyUsedPatterns;
    
    TMap<FGameplayTag, EDoppelgangerActionPattern> PatternTagToEnumMap;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Guard", meta = (AllowPrivateAccess = "true"))
    EDoppelgangerGuardState CurrentGuardState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Guard", meta = (AllowPrivateAccess = "true"))
    int32 CurrentGuardCount;

    FTimerHandle GuardBreakRecoveryTimer;
    FTimerHandle GuardCounterResetTimer;
};