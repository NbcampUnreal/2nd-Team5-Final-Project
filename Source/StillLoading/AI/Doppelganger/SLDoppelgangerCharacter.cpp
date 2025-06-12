#include "SLDoppelgangerCharacter.h"

#include "AIController.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/GamePlayTag/AIGamePlayTag.h"
#include "Character/SLPlayerCharacter.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Character/Item/SLItem.h"
#include "Components/CapsuleComponent.h"
#include "Controller/SLBaseAIController.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

ASLDoppelgangerCharacter::ASLDoppelgangerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    CurrentActionPattern = EDoppelgangerActionPattern::EDAP_None;
    MaxRecentPatternMemory = 1;
    CurrentGuardState = EDoppelgangerGuardState::EDGS_None;
    CurrentGuardCount = 0;
    MaxGuardCount = 3;
    GuardBreakDuration = 5.0f;
    GuardCounterResetDelay = 3.0f;
    ParryDuration = 1.5f;
    CurrentDashDegree = 0.0f;
    MinDashDistance = 200.0f;
    MaxDashDistance = 800.0f;
    OptimalCombatDistance = 400.0f;
    DashDuration = 2.0f;
    bIsAirHit = false;
    
    bCanBeExecuted = false;
    
    HitReactionMode = EHitReactionMode::EHRM_Threshold;
    HitDamageThreshold = 5.0f; 
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

bool ASLDoppelgangerCharacter::IsPlayerJumping() const
{
    if (!AIController)
    {
        return false;
    }
    
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        return false;
    }
    
    ASLPlayerCharacter* PlayerCharacter = Cast<ASLPlayerCharacter>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
    if (!PlayerCharacter)
    {
        return false;
    }
    
    UCharacterMovementComponent* PlayerMovement = PlayerCharacter->GetCharacterMovement();
    if (!PlayerMovement)
    {
        return false;
    }
    
    return PlayerMovement->IsFalling();
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
        SetIsLoop(true);
        StopGuardCounterResetTimer();
    }
    else if (NewPattern == EDoppelgangerActionPattern::EDAP_Parry)
    {
        CurrentGuardState = EDoppelgangerGuardState::EDGS_Parrying;
        SetIsLoop(false);
        StopGuardCounterResetTimer();
    }
    else if (NewPattern == EDoppelgangerActionPattern::EDAP_Guard_Break)
    {
        CurrentGuardState = EDoppelgangerGuardState::EDGS_GuardBroken;
        SetIsLoop(false);
        StopGuardCounterResetTimer();
    }
    else if (CurrentGuardState != EDoppelgangerGuardState::EDGS_GuardBroken && 
             CurrentGuardState != EDoppelgangerGuardState::EDGS_Parrying)
    {
        if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Guarding && CurrentGuardCount > 0)
        {
            StartGuardCounterResetTimer();
        }
        CurrentGuardState = EDoppelgangerGuardState::EDGS_None;
        SetIsLoop(false);
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
        RecentlyUsedPatterns.Reset();
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
    SpecialPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Attack_Air_To_Floor);
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
    
    AllPatterns.AddTag(SLAIGameplayTags::Doppelganger_Pattern_Dash);
    
    return AllPatterns;
}

void ASLDoppelgangerCharacter::OnGuardSuccess()
{
    if (CurrentGuardState != EDoppelgangerGuardState::EDGS_Guarding)
    {
        return;
    }

    TriggerParry();
    
    UE_LOG(LogTemp, Warning, TEXT("Guard Success! Triggering Parry!"));
}

void ASLDoppelgangerCharacter::TriggerParry()
{
    CurrentGuardState = EDoppelgangerGuardState::EDGS_Parrying;
    SetCurrentActionPattern(EDoppelgangerActionPattern::EDAP_Parry);
    
    CurrentGuardCount = 0;
    
    /*UE_LOG(LogTemp, Warning, TEXT("Parry Triggered!"));*/
    
    GetWorld()->GetTimerManager().SetTimer(ParryRecoveryTimer, this, &ASLDoppelgangerCharacter::FinishParry, ParryDuration * 2.0f, false);
}

void ASLDoppelgangerCharacter::FinishParry()
{
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Parrying)
    {
        CurrentGuardState = EDoppelgangerGuardState::EDGS_None;  
        SetCurrentActionPattern(EDoppelgangerActionPattern::EDAP_None);
        
        /*UE_LOG(LogTemp, Warning, TEXT("Parry Finished - Returning to Normal State"));*/
    }
}

bool ASLDoppelgangerCharacter::IsParrying() const
{
    return CurrentGuardState == EDoppelgangerGuardState::EDGS_Parrying;
}

void ASLDoppelgangerCharacter::OnParryAnimationEnd()
{
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Parrying)
    {
        if (ParryRecoveryTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(ParryRecoveryTimer);
        }
        
        FinishParry();
    }
}

void ASLDoppelgangerCharacter::TriggerGuardBreak()
{
    CurrentGuardState = EDoppelgangerGuardState::EDGS_GuardBroken;
    SetCurrentActionPattern(EDoppelgangerActionPattern::EDAP_Guard_Break);
    
    CurrentGuardCount = 0;
    
    UE_LOG(LogTemp, Warning, TEXT("Guard Break Triggered!"));
    
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

bool ASLDoppelgangerCharacter::IsGuardBroken() const
{
    return CurrentGuardState == EDoppelgangerGuardState::EDGS_GuardBroken;
}

void ASLDoppelgangerCharacter::PerformDash(float DashDegree)
{
    SetDashDegree(DashDegree);
    SetCurrentActionPattern(EDoppelgangerActionPattern::EDAP_Dash);
    
    GetWorld()->GetTimerManager().SetTimer(
        DashTimerHandle,
        [this]()
        {
            SetCurrentActionPattern(EDoppelgangerActionPattern::EDAP_None);
            UE_LOG(LogTemp, Warning, TEXT("Dash Completed"));
        },
        DashDuration,
        false
    );
    
    UE_LOG(LogTemp, Warning, TEXT("Performing Dash at %f degrees"), DashDegree);
}

void ASLDoppelgangerCharacter::PerformContextualDash()
{
    float ContextualDegree = CalculateContextualDashDegree();
    PerformDash(ContextualDegree);
}

void ASLDoppelgangerCharacter::SetDashDegree(float Degree)
{
    CurrentDashDegree = Degree;
}

float ASLDoppelgangerCharacter::GetDashDegree() const
{
    return CurrentDashDegree;
}

void ASLDoppelgangerCharacter::CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType)
{
    // 가드 브레이크 상태에서는 플레이어 공격 반응 처리 후 일반 피격 처리
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_GuardBroken)
    {
        OnPlayerAttackReceived(DamageCauser, DamageAmount, HitResult, AnimType);
        Super::CharacterHit(DamageCauser, DamageAmount, HitResult, AnimType);
        return;
    }
    
    // 패리 중일 때는 완전 무효화
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Parrying)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attack completely nullified by Parry! No damage and no hit state."));
        return;
    }
    
    // 가드 중이면 데미지는 막지만 히트 상태는 설정
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Guarding)
    {
        SetHitStateForGuard(DamageCauser, HitResult);
        OnGuardSuccess();
        UE_LOG(LogTemp, Warning, TEXT("Guard Success! Damage blocked but guard reaction triggered."));
        return;
    }
    
    if (CurrentGuardCount > 0)
    {
        StopGuardCounterResetTimer();
    }
    
    OnPlayerAttackReceived(DamageCauser, DamageAmount, HitResult, AnimType);
    
    Super::CharacterHit(DamageCauser, DamageAmount, HitResult, AnimType);
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
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Attack_Air_To_Floor, EDoppelgangerActionPattern::EDAP_Attack_Air_To_Floor);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Run_Attack_02, EDoppelgangerActionPattern::EDAP_Run_Attack_02);
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Dash_Air_Attack, EDoppelgangerActionPattern::EDAP_Dash_Air_Attack);
    
    // 대시 패턴 매핑
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Dash, EDoppelgangerActionPattern::EDAP_Dash);
    
    // 방어 및 지원 매핑
    PatternTagToEnumMap.Add(SLAIGameplayTags::Doppelganger_Pattern_Guard, EDoppelgangerActionPattern::EDAP_Guard);
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

float ASLDoppelgangerCharacter::CalculateContextualDashDegree() const
{
    if (IsPlayerAttacking())
    {
        return GetRandomEvasiveAngle();
    }
    else if (IsTargetTooClose())
    {
        return 180.0f;
    }
    else if (IsTargetTooFar())
    {
        return 0.0f;
    }
    else
    {
        return GetFlankingAngle();
    }
}

bool ASLDoppelgangerCharacter::IsPlayerAttacking() const
{
    if (ASLBaseAIController* BaseController = Cast<ASLBaseAIController>(AIController))
    {
        return BaseController->IsPlayerAttacking();
    }
    
    return false;
}

bool ASLDoppelgangerCharacter::IsTargetTooClose() const
{
    if (!AIController)
    {
        return false;
    }
    
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        return false;
    }
    
    AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
    if (!TargetActor)
    {
        return false;
    }
    
    float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
    return Distance < MinDashDistance;
}

bool ASLDoppelgangerCharacter::IsTargetTooFar() const
{
    if (!AIController)
    {
        return false;
    }
    
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        return false;
    }
    
    AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
    if (!TargetActor)
    {
        return false;
    }
    
    float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
    return Distance > MaxDashDistance;
}

float ASLDoppelgangerCharacter::GetRandomEvasiveAngle() const
{
    TArray<float> EvasiveAngles = {90.0f, -90.0f, 180.0f};
    int32 RandomIndex = FMath::RandRange(0, EvasiveAngles.Num() - 1);
    return EvasiveAngles[RandomIndex];
}

float ASLDoppelgangerCharacter::GetFlankingAngle() const
{
    return FMath::RandBool() ? 90.0f : -90.0f;
}

void ASLDoppelgangerCharacter::SetHitStateForGuard(AActor* DamageCauser, const FHitResult& HitResult)
{
    if (!DamageCauser)
    {
        return;
    }
    
    FVector AttackerLocation = DamageCauser->GetActorLocation();
    FVector DirectionVector = AttackerLocation - GetActorLocation();
    DirectionVector.Normalize();
    
    FVector LocalHitDirection = GetActorTransform().InverseTransformVectorNoScale(DirectionVector);

    float AbsX = FMath::Abs(LocalHitDirection.X);
    float AbsY = FMath::Abs(LocalHitDirection.Y);

    if (AbsY > AbsX)
    {
        HitDirection = (LocalHitDirection.Y > 0) ? EHitDirection::EHD_Right : EHitDirection::EHD_Left;
    }
    else 
    {
        HitDirection = (LocalHitDirection.X > 0) ? EHitDirection::EHD_Front : EHitDirection::EHD_Back;
    }

    HitDirectionVector = LocalHitDirection;
    
    SetHitState(true, 0.5f);
    
    if (HitEffectComponent)
    {
        FVector EffectLocation = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.Location;
        
        if (EffectLocation.IsZero())
        {
            EffectLocation = GetActorLocation();
            EffectLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f;
        }
        
        HitEffectComponent->SetWorldLocation(EffectLocation);
        HitEffectComponent->ActivateSystem();
    }
}

void ASLDoppelgangerCharacter::OnPlayerAttackReceived(AActor* Causer, float Damage, const FHitResult& HitResult, EAttackAnimType AnimType)
{
    if (!Causer)
    {
        return;
    }

    ProcessPlayerSpecificReaction(Causer, AnimType);
}

void ASLDoppelgangerCharacter::ProcessPlayerSpecificReaction(AActor* Causer, EAttackAnimType AnimType)
{
    if (!Causer)
    {
        return;
    }
    
    // 공중 상태 체크
    const float GroundDistance = GetCharacterMovement()->CurrentFloor.FloorDist;
    bool bIsCurrentlyInAir = GetCharacterMovement()->IsFalling() && GroundDistance > 50.0f;
    bool bIsAirborneAttack = (AnimType == EAttackAnimType::AAT_Airborn || 
                             AnimType == EAttackAnimType::AAT_SpecialAttack1 ||
                             AnimType == EAttackAnimType::AAT_SpecialAttack2 ||
                             AnimType == EAttackAnimType::AAT_SpecialAttack3 ||
                             AnimType == EAttackAnimType::AAT_Skill1);

    // AirHit 상태를 클래스 멤버 변수로 설정
    SetIsAirHit(bIsCurrentlyInAir || bIsAirborneAttack);

    // 기본 넉백 처리
    FVector PushDirection = GetActorLocation() - Causer->GetActorLocation();
    PushDirection.Z = 0.0f;
    PushDirection.Normalize();

    // 넉백 강도 결정
    float KnockbackStrength = 100.0f;
    float UpwardForce = 50.0f; 

    if (AnimType == EAttackAnimType::AAT_SpecialAttack1 || 
        AnimType == EAttackAnimType::AAT_SpecialAttack2 || 
        AnimType == EAttackAnimType::AAT_SpecialAttack3)
    {
        KnockbackStrength = 300.0f;
        UpwardForce = 700.0f; 
    }

    // 공격 타입별 특수 처리 및 Launch 벡터 계산
    FVector LaunchVelocity = PushDirection * KnockbackStrength;
    
    switch (AnimType)
    {
    case EAttackAnimType::AAT_NormalAttack1:
    case EAttackAnimType::AAT_NormalAttack2:
    case EAttackAnimType::AAT_NormalAttack3:
        {
            LaunchVelocity.Z = UpwardForce;
            
            // 공중 상태에서 수직 속도 제거 
            if (GetIsAirHit())
            {
                GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
                {
                    FVector Velocity = GetCharacterMovement()->Velocity;
                    Velocity.Z = 0.f;
                    GetCharacterMovement()->Velocity = Velocity;
                });
            }
            break;
        }

    case EAttackAnimType::AAT_SpecialAttack1:
    case EAttackAnimType::AAT_SpecialAttack2:
    case EAttackAnimType::AAT_SpecialAttack3:
        {
            LaunchVelocity.Z = 100.f;  // 강한 상승력
            break;
        }

    case EAttackAnimType::AAT_AirAttack1:
    case EAttackAnimType::AAT_AirAttack2:
    case EAttackAnimType::AAT_AirAttack3:
        {
            LaunchVelocity.Z = UpwardForce * 0.7f;  // 공중 공격은 약간 낮은 상승력
            break;
        }

    case EAttackAnimType::AAT_Airborn:
        {
            LaunchVelocity.Z = 700.f * 1.5f;  // 에어본은 높은 상승력
            break;
        }

    case EAttackAnimType::AAT_Skill1:
        {
            LaunchVelocity.Z = UpwardForce * 1.2f;  // 스킬1은 상향 공격
            break;
        }

    case EAttackAnimType::AAT_Skill2:
        {
            LaunchVelocity.Z = UpwardForce * 0.5f;  // 스킬2는 낮은 상승력
            break;
        }

    default:
        {
            LaunchVelocity.Z = UpwardForce;
            break;
        }
    }

    // 넉백 적용
    LaunchCharacter(LaunchVelocity, true, false);
}

void ASLDoppelgangerCharacter::SetHitState(bool bNewIsHit, float AutoResetTime)
{
    
    
    if (HitStateResetTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HitStateResetTimer);
        UE_LOG(LogTemp, Warning, TEXT("Cleared existing hit state timer"));
    }
    
    bIsHit = bNewIsHit;
    
    if (AIController)
    {
        if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
        {
            BlackboardComponent->SetValueAsBool(FName("IsHit"), bNewIsHit);
        }
    }
    
    if (bNewIsHit && AutoResetTime > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(HitStateResetTimer, [this]()
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit state reset by timer"));
            bIsHit = false;
            if (AIController)
            {
                if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
                {
                    BB->SetValueAsBool(FName("IsHit"), false);
                }
            }
        }, AutoResetTime, false);
    }
}

void ASLDoppelgangerCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    
    // 착지 시 AirHit 상태 해제
    //SetIsAirHit(false);
}