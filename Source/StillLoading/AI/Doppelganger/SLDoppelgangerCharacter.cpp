// Fill out your copyright notice in the Description page of Project Settings.

#include "SLDoppelgangerCharacter.h"

#include "AI/GamePlayTag/AIGamePlayTag.h"
#include "Character/Item/SLItem.h"
#include "Engine/Engine.h"

ASLDoppelgangerCharacter::ASLDoppelgangerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentActionPattern = EDoppelgangerActionPattern::EDAP_None;
    MaxRecentPatternMemory = 3;
    CurrentGuardState = EDoppelgangerGuardState::EDGS_None;
    CurrentGuardCount = 0;
    MaxGuardCount = 3;
    GuardBreakDuration = 5.0f;
    GuardCounterResetDelay = 3.0f;
}

void ASLDoppelgangerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Sword 스폰 및 장착
    if (SwordClass)
    {
        Sword = GetWorld()->SpawnActor<ASLItem>(SwordClass, GetActorLocation(), GetActorRotation());
        AttachItemToHand(Sword, TEXT("r_weapon_socket"));
        Sword->SetOwner(this);
    }

    // Shield 스폰 및 장착
    if (ShieldClass)
    {
        Shield = GetWorld()->SpawnActor<ASLItem>(ShieldClass, GetActorLocation(), GetActorRotation());
        AttachItemToHand(Shield, TEXT("l_weapon_socket"));
        Shield->SetOwner(this);
    }

    InitializePatternMapping();
}

void ASLDoppelgangerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

EDoppelgangerActionPattern ASLDoppelgangerCharacter::GetCurrentActionPattern() const
{
    return CurrentActionPattern;
}

void ASLDoppelgangerCharacter::SetCurrentActionPattern(EDoppelgangerActionPattern NewPattern)
{
    CurrentActionPattern = NewPattern;
    
    // 가드 상태 업데이트
    if (NewPattern == EDoppelgangerActionPattern::EDAP_Guard)
    {
        CurrentGuardState = EDoppelgangerGuardState::EDGS_Guarding;
        // 가드 시작 시 카운터 리셋 타이머 중지
        StopGuardCounterResetTimer();
    }
    else if (NewPattern == EDoppelgangerActionPattern::EDAP_Guard_Break)
    {
        CurrentGuardState = EDoppelgangerGuardState::EDGS_GuardBroken;
        StopGuardCounterResetTimer();
    }
    else if (CurrentGuardState != EDoppelgangerGuardState::EDGS_GuardBroken)
    {
        // 가드 상태에서 벗어날 때
        if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Guarding && CurrentGuardCount > 0)
        {
            // 가드 해제 시 일정 시간 후 카운터 리셋 타이머 시작
            StartGuardCounterResetTimer();
        }
        CurrentGuardState = EDoppelgangerGuardState::EDGS_None;
    }
}

EDoppelgangerActionPattern ASLDoppelgangerCharacter::SelectRandomPattern()
{
    FGameplayTagContainer AllPatterns = GetAllAttackPatterns();
    return SelectRandomPatternFromTags(AllPatterns);
}

EDoppelgangerActionPattern ASLDoppelgangerCharacter::SelectRandomPatternFromTags(const FGameplayTagContainer& PatternTags)
{
    if (PatternTags.IsEmpty())
    {
        return EDoppelgangerActionPattern::EDAP_None;
    }

    // 최근 사용한 패턴 제외
    FGameplayTagContainer FilteredPatterns;
    TArray<FGameplayTag> TagArray;
    PatternTags.GetGameplayTagArray(TagArray);

    for (const FGameplayTag& Tag : TagArray)
    {
        if (!RecentlyUsedPatterns.HasTag(Tag))
        {
            FilteredPatterns.AddTag(Tag);
        }
    }

    // 필터링 후 패턴이 없으면 전체에서 선택
    if (FilteredPatterns.IsEmpty())
    {
        FilteredPatterns = PatternTags;
        RecentlyUsedPatterns.Reset(); // 최근 사용 목록 초기화
    }

    // 랜덤 선택
    TArray<FGameplayTag> FinalTagArray;
    FilteredPatterns.GetGameplayTagArray(FinalTagArray);
    
    int32 RandomIndex = FMath::RandRange(0, FinalTagArray.Num() - 1);
    FGameplayTag SelectedTag = FinalTagArray[RandomIndex];

    // 최근 사용 패턴에 추가
    RecentlyUsedPatterns.AddTag(SelectedTag);
    
    // 최대 개수 제한
    TArray<FGameplayTag> RecentArray;
    RecentlyUsedPatterns.GetGameplayTagArray(RecentArray);
    if (RecentArray.Num() > MaxRecentPatternMemory)
    {
        RecentlyUsedPatterns.RemoveTag(RecentArray[0]);
    }

    return GetActionPatternForTag(SelectedTag);
}

FGameplayTagContainer ASLDoppelgangerCharacter::GetComboAttackPatterns() const
{
    FGameplayTagContainer ComboPatterns;
    ComboPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_01);
    ComboPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_02);
    ComboPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_03);
    ComboPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_04);
    ComboPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_Air);
    return ComboPatterns;
}

FGameplayTagContainer ASLDoppelgangerCharacter::GetSpecialAttackPatterns() const
{
    FGameplayTagContainer SpecialPatterns;
    SpecialPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Attack_Up_01);
    SpecialPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Attack_Up_Floor_To_Air_02);
    SpecialPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Run_Attack_02);
    SpecialPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Dash_Air_Attack);
    return SpecialPatterns;
}

FGameplayTagContainer ASLDoppelgangerCharacter::GetDefensePatterns() const
{
    FGameplayTagContainer DefensePatterns;
    DefensePatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Guard);
    DefensePatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Parry);
    DefensePatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Buff_Cast);
    return DefensePatterns;
}

FGameplayTagContainer ASLDoppelgangerCharacter::GetAllAttackPatterns() const
{
    FGameplayTagContainer AllPatterns;
    AllPatterns.AppendTags(GetComboAttackPatterns());
    AllPatterns.AppendTags(GetSpecialAttackPatterns());
    AllPatterns.AppendTags(GetDefensePatterns());
    return AllPatterns;
}

void ASLDoppelgangerCharacter::OnGuardSuccess()
{
    if (CurrentGuardState != EDoppelgangerGuardState::EDGS_Guarding)
    {
        return;
    }

    CurrentGuardCount++;
    
    // 가드 성공 시 카운터 리셋 타이머 재시작
    StartGuardCounterResetTimer();
    
    UE_LOG(LogTemp, Warning, TEXT("Guard Success! Count: %d/%d"), CurrentGuardCount, MaxGuardCount);

    if (CurrentGuardCount >= MaxGuardCount)
    {
        TriggerGuardBreak();
    }
}

void ASLDoppelgangerCharacter::TriggerGuardBreak()
{
    CurrentGuardState = EDoppelgangerGuardState::EDGS_GuardBroken;
    SetCurrentActionPattern(EDoppelgangerActionPattern::EDAP_Guard_Break);
    
    // 가드 카운터 리셋
    CurrentGuardCount = 0;
    
    UE_LOG(LogTemp, Warning, TEXT("Guard Break Triggered!"));
    
    // 일정 시간 후 가드 브레이크에서 회복
    GetWorld()->GetTimerManager().SetTimer(GuardBreakRecoveryTimer, this, &ASLDoppelgangerCharacter::RecoverFromGuardBreak, GuardBreakDuration, false);
}

void ASLDoppelgangerCharacter::RecoverFromGuardBreak()
{
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_GuardBroken)
    {
        CurrentGuardState = EDoppelgangerGuardState::EDGS_None;
        SetCurrentActionPattern(EDoppelgangerActionPattern::EDAP_None);
        
        UE_LOG(LogTemp, Warning, TEXT("Recovered from Guard Break"));
    }
}

void ASLDoppelgangerCharacter::ResetGuardCounter()
{
    CurrentGuardCount = 0;
    StopGuardCounterResetTimer();
    UE_LOG(LogTemp, Log, TEXT("Guard Counter Reset"));
}

void ASLDoppelgangerCharacter::StartGuardCounterResetTimer()
{
    StopGuardCounterResetTimer();
    GetWorld()->GetTimerManager().SetTimer(GuardCounterResetTimer, this, &ASLDoppelgangerCharacter::ResetGuardCounterWithDelay, GuardCounterResetDelay, false);
}

void ASLDoppelgangerCharacter::StopGuardCounterResetTimer()
{
    if (GuardCounterResetTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(GuardCounterResetTimer);
    }
}

void ASLDoppelgangerCharacter::ResetGuardCounterWithDelay()
{
    if (CurrentGuardCount > 0)
    {
        ResetGuardCounter();
        UE_LOG(LogTemp, Log, TEXT("Guard Counter Reset due to timeout"));
    }
}

bool ASLDoppelgangerCharacter::IsGuardBroken() const
{
    return CurrentGuardState == EDoppelgangerGuardState::EDGS_GuardBroken;
}

void ASLDoppelgangerCharacter::InitializePatternMapping()
{
    PatternTagToEnumMap.Reset();
    
    // 콤보 공격 매핑
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_01, EDoppelgangerActionPattern::EDAP_Combo_Attack_01);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_02, EDoppelgangerActionPattern::EDAP_Combo_Attack_02);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_03, EDoppelgangerActionPattern::EDAP_Combo_Attack_03);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_04, EDoppelgangerActionPattern::EDAP_Combo_Attack_04);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Combo_Attack_Air, EDoppelgangerActionPattern::EDAP_Combo_Attack_Air);
    
    // 특수 공격 매핑
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Attack_Up_01, EDoppelgangerActionPattern::EDAP_Attack_Up_01);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Attack_Up_Floor_To_Air_02, EDoppelgangerActionPattern::EDAP_Attack_Up_Floor_To_Air_02);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Run_Attack_02, EDoppelgangerActionPattern::EDAP_Run_Attack_02);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Dash_Air_Attack, EDoppelgangerActionPattern::EDAP_Dash_Air_Attack);
    
    // 방어 및 지원 매핑
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Guard, EDoppelgangerActionPattern::EDAP_Guard);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Parry, EDoppelgangerActionPattern::EDAP_Parry);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Buff_Cast, EDoppelgangerActionPattern::EDAP_Buff_Cast);
}

EDoppelgangerActionPattern ASLDoppelgangerCharacter::GetActionPatternForTag(const FGameplayTag& Tag) const
{
    if (const EDoppelgangerActionPattern* FoundPattern = PatternTagToEnumMap.Find(Tag))
    {
        return *FoundPattern;
    }
    return EDoppelgangerActionPattern::EDAP_None;
}

void ASLDoppelgangerCharacter::AttachItemToHand(AActor* ItemActor, const FName SocketName) const
{
    if (!ItemActor || !GetMesh())
    {
        return;
    }

    ItemActor->AttachToComponent(
        GetMesh(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        SocketName
    );
}

void ASLDoppelgangerCharacter::CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType)
{
    // 가드 브레이크 상태에서는 가드 불가
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_GuardBroken)
    {
        Super::CharacterHit(DamageCauser, DamageAmount, HitResult, AnimType);
        return;
    }
    
    // 가드 중이면 가드 성공 처리
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Guarding)
    {
        OnGuardSuccess();
        
        // 가드 성공 시 데미지 감소
        float ReducedDamage = DamageAmount * 0.1f; // 90% 데미지 감소
        Super::CharacterHit(DamageCauser, ReducedDamage, HitResult, AnimType);
        
        UE_LOG(LogTemp, Warning, TEXT("Guard Success! Damage reduced from %f to %f"), DamageAmount, ReducedDamage);
    }
    else
    {
        // 가드 중이 아니면 일반 데미지 처리
        // 가드 카운터가 있고 가드하지 않는 상태에서 데미지를 받으면 타이머 중지하고 카운터 유지
        if (CurrentGuardCount > 0)
        {
            StopGuardCounterResetTimer();
        }
        
        Super::CharacterHit(DamageCauser, DamageAmount, HitResult, AnimType);
    }
}