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
    MaxRecentPatternMemory = 3;
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
    
    // 점프 공격 관련 초기화
    JumpAttackRange = 500.0f;
    JumpAttackHeight = 300.0f;
    PlayerJumpCheckInterval = 0.1f;
    LastPlayerJumpCheckTime = 0.0f;
    bPlayerWasJumpingLastFrame = false;
    
    bCanBeExecuted = false;
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
    
    // 주기적으로 플레이어 점프 상태 확인
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastPlayerJumpCheckTime >= PlayerJumpCheckInterval)
    {
        bool bPlayerCurrentlyJumping = IsPlayerJumping();
        
        // 플레이어가 새로 점프를 시작했을 때 점프 공격 고려
        if (bPlayerCurrentlyJumping && !bPlayerWasJumpingLastFrame)
        {
            if (ShouldPerformJumpAttack())
            {
                PerformJumpAttack();
            }
        }
        
        bPlayerWasJumpingLastFrame = bPlayerCurrentlyJumping;
        LastPlayerJumpCheckTime = CurrentTime;
    }
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
    
    // 플레이어의 이동 컴포넌트를 통해 점프 상태 확인
    UCharacterMovementComponent* PlayerMovement = PlayerCharacter->GetCharacterMovement();
    if (!PlayerMovement)
    {
        return false;
    }
    
    // 떨어지고 있거나 점프 중인 상태 확인
    return PlayerMovement->IsFalling();
}

void ASLDoppelgangerCharacter::PerformJumpAttack()
{
    if (!CanAIJump())
    {
        UE_LOG(LogTemp, Warning, TEXT("Doppelganger cannot jump for attack"));
        return;
    }
    
    // 점프 공격 패턴 선택
    EDoppelgangerActionPattern JumpPattern = SelectJumpAttackPattern();
    SetCurrentActionPattern(JumpPattern);
    
    // 플레이어 방향으로 점프
    JumpTowardsPlayer();
    
}

EDoppelgangerActionPattern ASLDoppelgangerCharacter::SelectJumpAttackPattern() const
{
    // 점프 공격에 적합한 패턴들
    TArray<EDoppelgangerActionPattern> JumpAttackPatterns = {
        EDoppelgangerActionPattern::EDAP_Combo_Attack_Air,
        EDoppelgangerActionPattern::EDAP_Attack_Up_Floor_To_Air_02,
        EDoppelgangerActionPattern::EDAP_Dash_Air_Attack
    };
    
    // 랜덤하게 선택
    int32 RandomIndex = FMath::RandRange(0, JumpAttackPatterns.Num() - 1);
    return JumpAttackPatterns[RandomIndex];
}

void ASLDoppelgangerCharacter::JumpTowardsPlayer()
{
    FVector JumpTarget = CalculateJumpTargetLocation();
    
    if (JumpTarget.IsZero())
    {
        // 타겟이 없으면 기본 점프
        AIJump();
        return;
    }
    
    // 플레이어 방향으로 회전
    FVector DirectionToTarget = (JumpTarget - GetActorLocation()).GetSafeNormal();
    DirectionToTarget.Z = 0.0f; // 수평 방향만
    
    if (!DirectionToTarget.IsZero())
    {
        FRotator TargetRotation = FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();
        SetActorRotation(TargetRotation);
    }
    
    // 점프 수행
    AIJump();
    
    // 점프 후 약간의 전진 속도 추가 (선택사항)
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        FVector LaunchVelocity = DirectionToTarget * 300.0f; // 전진 속도
        LaunchVelocity.Z = MovementComp->JumpZVelocity * 0.8f; // 점프 높이 조절
        
        MovementComp->Launch(LaunchVelocity);
    }
}

FVector ASLDoppelgangerCharacter::CalculateJumpTargetLocation() const
{
    if (!AIController)
    {
        return FVector::ZeroVector;
    }
    
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        return FVector::ZeroVector;
    }
    
    AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
    if (!TargetActor)
    {
        return FVector::ZeroVector;
    }
    
    FVector PlayerLocation = TargetActor->GetActorLocation();
    FVector MyLocation = GetActorLocation();
    
    // 플레이어 근처의 위치를 타겟으로 설정 (너무 가깝지 않게)
    FVector DirectionToPlayer = (PlayerLocation - MyLocation).GetSafeNormal();
    FVector TargetLocation = PlayerLocation - (DirectionToPlayer * 200.0f); // 플레이어로부터 200유닛 떨어진 위치
    
    return TargetLocation;
}

bool ASLDoppelgangerCharacter::ShouldPerformJumpAttack() const
{
    // 이미 점프 중이거나 공격 중이면 점프 공격 안함
    if (bIsJumping || bIsAttacking)
    {
        return false;
    }
    
    // 가드나 패리 중이면 점프 공격 안함
    if (CurrentGuardState != EDoppelgangerGuardState::EDGS_None)
    {
        return false;
    }
    
    // 플레이어와의 거리 확인
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
    
    float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
    
    // 적절한 거리에 있을 때만 점프 공격 (너무 가깝거나 멀면 안함)
    bool bInRange = (DistanceToPlayer >= 200.0f && DistanceToPlayer <= JumpAttackRange);
    
    // 30% 확률로 점프 공격 수행 (너무 자주 하지 않게)
    bool bRandomChance = FMath::RandRange(1, 100) <= 30;
    
    return bInRange && bRandomChance;
}

// 기존 함수들은 동일하게 유지...
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
    
    // 대시 패턴 직접 추가
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
    
    UE_LOG(LogTemp, Warning, TEXT("Parry Triggered!"));
    
    GetWorld()->GetTimerManager().SetTimer(ParryRecoveryTimer, this, &ASLDoppelgangerCharacter::FinishParry, ParryDuration * 2.0f, false);
}

void ASLDoppelgangerCharacter::FinishParry()
{
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Parrying)
    {
        CurrentGuardState = EDoppelgangerGuardState::EDGS_None;
        SetCurrentActionPattern(EDoppelgangerActionPattern::EDAP_None);
        
        UE_LOG(LogTemp, Warning, TEXT("Parry Finished"));
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
    
    // 대시 종료 타이머 설정
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
    // 가드 브레이크 상태에서는 일반 피격 처리
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_GuardBroken)
    {
        Super::CharacterHit(DamageCauser, DamageAmount, HitResult, AnimType);
        return;
    }
    
    // 패리 중일 때는 완전 무효화 (데미지도 히트 상태도 없음)
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Parrying)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attack completely nullified by Parry! No damage and no hit state."));
        return;
    }
    
    // 가드 중이면 데미지는 막지만 히트 상태는 설정 (가드 리액션)
    if (CurrentGuardState == EDoppelgangerGuardState::EDGS_Guarding)
    {
        SetHitStateForGuard(DamageCauser, HitResult);
        OnGuardSuccess();
        UE_LOG(LogTemp, Warning, TEXT("Guard Success! Damage blocked but guard reaction triggered."));
        return;
    }
    
    // 가드 중이 아니면 일반 데미지 처리
    if (CurrentGuardCount > 0)
    {
        StopGuardCounterResetTimer();
    }
    
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
    
    // 공격자 방향 계산
    FVector AttackerLocation = DamageCauser->GetActorLocation();
    FVector DirectionVector = AttackerLocation - GetActorLocation();
    DirectionVector.Normalize();
    
    FVector LocalHitDirection = GetActorTransform().InverseTransformVectorNoScale(DirectionVector);

    // 히트 방향 결정
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
    
    // Hit 상태 설정
    bIsHit = true;

    // 히트 상태 리셋 타이머
    FTimerHandle HitResetTimer;
    GetWorld()->GetTimerManager().SetTimer(
        HitResetTimer,
        [this]()
        {
            bIsHit = false;
        },
        0.5f,
        false
    );
    
    // 히트 이펙트 재생 (가드/패리 시에도 이펙트 표시)
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
    
    UE_LOG(LogTemp, Warning, TEXT("Hit state set for guard/parry - Direction: %s"), 
           *UEnum::GetValueAsString(HitDirection));
}