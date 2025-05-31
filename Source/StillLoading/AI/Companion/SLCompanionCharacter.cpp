// SLCompanionCharacter.cpp
#include "SLCompanionCharacter.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/SLCompanionPatternData.h"
#include "AI/GamePlayTag/AIGamePlayTag.h"
#include "AI/Projectile/SLAIProjectile.h"
#include "AI/Projectile/SLHomingProjectile.h"
#include "AI/SLMonster/SLMonster.h"
#include "Components/CapsuleComponent.h"
#include "Controller/SLBaseAIController.h"
#include "Controller/SLCompanionAIController.h"

FSLCompanionActionPatternMappingRow::FSLCompanionActionPatternMappingRow()
{
    ActionPattern = ECompanionActionPattern::ECAP_None;
    DistanceType = ECompanionDistanceType::ECDT_Melee;
    bIsBattleMagePattern = true;
    Priority = 0;
}

ASLCompanionCharacter::ASLCompanionCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    
    bCanBeExecuted = false;
    IsHitReaction = true;
    
    ProjectileSocketName = "hand_r";
    AutoStunInterval = 10.0f;
    StunDuration = 3.0f;
    bIsInCombat = false;
    IsBattleMage = false;

    MeleeRangeThreshold = 200.0f;
    MidRangeThreshold = 500.0f;
    LongRangeThreshold = 800.0f;
    
    MaxRecentPatternMemory = 3;
    PatternData = nullptr;
}

void ASLCompanionCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ASLCompanionCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASLCompanionCharacter::SetCombatMode(bool bInCombat)
{
    bIsInCombat = bInCombat;
    
    if (AIController)
    {
        if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
        {
            BlackboardComponent->SetValueAsBool(FName("IsInCombat"), bIsInCombat);
        }

        ASLCompanionAIController* CompanionAIController = Cast<ASLCompanionAIController>(AIController);
        if (CompanionAIController)
        {
            if (bIsInCombat)
            {
                CompanionAIController->UpdateSightRadius(1000.0f, 1200.0f);
            }
            else
            {
                CompanionAIController->UpdateSightRadius(200.0f, 300.0f);
            }
        }
    }
}

bool ASLCompanionCharacter::HasGameplayTag(const FGameplayTag& TagToCheck) const
{
    return CurrentGameplayTags.HasTag(TagToCheck);
}

void ASLCompanionCharacter::FireProjectile(EAttackAnimType AttackAnimType)
{
    if (!ProjectileClass || !AIController) return;
    
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent) return;
    
    AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
    if (!Target) return;
    
    FVector SpawnLocation = GetMesh()->GetSocketLocation(ProjectileSocketName);
    FRotator SpawnRotation = (Target->GetActorLocation() - SpawnLocation).Rotation();
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
    
    if (ASLHomingProjectile* Projectile = GetWorld()->SpawnActor<ASLHomingProjectile>(
        ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
    {
        Projectile->SetHomingTarget(Target);
        Projectile->SetupSpawnedProjectile(AttackAnimType, 1500.0f);
    }
}

bool ASLCompanionCharacter::GetIsBattleMage()
{
    return IsBattleMage;
}

void ASLCompanionCharacter::SetIsBattleMage(bool bInBattleMage)
{
    IsBattleMage = bInBattleMage;
}

void ASLCompanionCharacter::CastStunSpell(AActor* Target)
{
    if (!Target) return;
    
    ASLAIBaseCharacter* EnemyCharacter = Cast<ASLAIBaseCharacter>(Target);
    if (!EnemyCharacter || EnemyCharacter->GetIsDead()) return;
    
    if (StunEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            StunEffect,
            Target->GetActorLocation(),
            FRotator::ZeroRotator,
            FVector(1.0f),
            true,
            true,
            ENCPoolMethod::AutoRelease
        );
    }
    
    FTimerHandle StunTimer;
    GetWorld()->GetTimerManager().SetTimer(StunTimer, [EnemyCharacter, this]()
    {
        if (IsValid(EnemyCharacter) && !EnemyCharacter->GetIsDead())
        {
            if (USLAICharacterAnimInstance* AnimInstance = Cast<USLAICharacterAnimInstance>(EnemyCharacter->GetMesh()->GetAnimInstance()))
            {
                AnimInstance->SetIsStun(false);
            }
        }
    }, StunDuration, false);
}

void ASLCompanionCharacter::AutoStunNearbyEnemy()
{
    if (!bIsInCombat || !AIController) return;
    
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent) return;
    
    AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));
    if (Target)
    {
        CastStunSpell(Target);
    }
}

void ASLCompanionCharacter::CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType)
{
    if (DamageCauser && IsHitReaction && AnimInstancePtr)
    {
        if (USLAICharacterAnimInstance* SLAIAnimInstance = Cast<USLAICharacterAnimInstance>(AnimInstancePtr.Get()))
        {
            FVector AttackerLocation = DamageCauser->GetActorLocation();
            FVector DirectionVector = AttackerLocation - GetActorLocation();
            DirectionVector.Normalize();
            
            FVector LocalHitDirection = GetActorTransform().InverseTransformVectorNoScale(DirectionVector);

            EHitDirection HitDir;
            float AbsX = FMath::Abs(LocalHitDirection.X);
            float AbsY = FMath::Abs(LocalHitDirection.Y);

            if (AbsY > AbsX)
            {
                HitDir = (LocalHitDirection.Y > 0) ? EHitDirection::EHD_Right : EHitDirection::EHD_Left;
            }
            else 
            {
                HitDir = (LocalHitDirection.X > 0) ? EHitDirection::EHD_Front : EHitDirection::EHD_Back;
            }
            
            SLAIAnimInstance->SetHitDirection(HitDir);
            SLAIAnimInstance->SetIsHit(true);
        }
    }

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

/**
 * 특정 거리값에 따라 사용할 수 있는 행동 패턴 태그 컨테이너를 반환합니다.
 * 거리 값은 근접, 중거리, 장거리, 매우 장거리 기준으로 구분되며, 이를 기반으로 적합한 패턴을 필터링합니다.
 *
 * @param DistanceToTarget 타겟까지의 거리. 이 값에 따라 적합한 거리 유형이 결정됩니다.
 * @return 해당 거리 조건에 맞는 행동 패턴 태그 컨테이너. 유효한 패턴 데이터가 없을 경우 빈 컨테이너를 반환합니다.
 */
FGameplayTagContainer ASLCompanionCharacter::GetAvailablePatternsByDistance(float DistanceToTarget) const
{
    if (!PatternData)
    {
        return FGameplayTagContainer();
    }

    ECompanionDistanceType DistanceType;
    if (DistanceToTarget <= MeleeRangeThreshold)
    {
        DistanceType = ECompanionDistanceType::ECDT_Melee;
    }
    else if (DistanceToTarget <= MidRangeThreshold)
    {
        DistanceType = ECompanionDistanceType::ECDT_MidRange;
    }
    else if (DistanceToTarget <= LongRangeThreshold)
    {
        DistanceType = ECompanionDistanceType::ECDT_LongRange;
    }
    else
    {
        DistanceType = ECompanionDistanceType::ECDT_VeryLong;
    }

    return PatternData->GetPatternsByDistanceTypeAsContainer(DistanceType, IsBattleMage);
}

/**
 * 입력된 태그 목록에서 적합한 행동 패턴을 필터링하고, 무작위로 패턴을 선택하여 반환합니다.
 * 특정 캐릭터 유형과 최근 사용 이력을 고려하여 적절한 패턴을 선택합니다.
 *
 * @param PatternTags 행동 패턴 태그의 컨테이너. 선택 가능한 태그 목록을 제공합니다.
 * @return 무작위로 선택된 행동 패턴. 적합한 패턴이 없는 경우 ECAP_None을 반환합니다.
 */
ECompanionActionPattern ASLCompanionCharacter::SelectRandomPatternFromTags(const FGameplayTagContainer& PatternTags)
{
    if (PatternTags.IsEmpty() || !PatternData)
    {
        return ECompanionActionPattern::ECAP_None;
    }

    // 캐릭터 타입에 맞게 필터링
    FGameplayTagContainer ValidPatterns = PatternData->FilterPatternsByCharacterType(PatternTags, IsBattleMage);
    
    if (ValidPatterns.IsEmpty())
    {
        return ECompanionActionPattern::ECAP_None;
    }

    // 최근 사용한 패턴 제외
    FGameplayTagContainer FilteredPatterns;
    TArray<FGameplayTag> TagArray;
    ValidPatterns.GetGameplayTagArray(TagArray);

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
        FilteredPatterns = ValidPatterns;
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

    return PatternData->GetActionPatternForTag(SelectedTag);
}

/**
 * 주어진 거리 값과 태그 컨테이너를 기반으로 적합한 행동 패턴을 랜덤하게 선택하여 반환합니다.
 * 선택 과정에서 캐릭터 타입에 맞지 않는 패턴은 필터링되며, 최근 사용한 패턴은 가능한 제외됩니다.
 * 적합한 패턴이 없는 경우 예외 처리가 이루어지며, 최근 사용한 패턴 기록은 설정된 최대 개수를 초과하지 않도록 관리됩니다.
 *
 * @param PatternTags 사용 가능한 패턴 태그의 컨테이너입니다. 이 컨테이너의 태그에서 선택이 이루어집니다.
 * @param DistanceToTarget 타겟까지의 거리. 해당 값은 거리 기반 패턴 필터링에 사용됩니다.
 * @return 선택된 ECompanionActionPattern 값. 적합한 패턴이 없을 경우 ECAP_None 또는 예외처리 값을 반환합니다.
 */
ECompanionActionPattern ASLCompanionCharacter::SelectRandomPatternFromTagsWithDistance(const FGameplayTagContainer& PatternTags, float DistanceToTarget)
{
    if (PatternTags.IsEmpty() || !PatternData)
    {
        return ECompanionActionPattern::ECAP_None;
    }

    // 거리에 맞는 패턴들 가져오기
    FGameplayTagContainer DistancePatterns = GetAvailablePatternsByDistance(DistanceToTarget);
    
    // 교집합 구하기
    FGameplayTagContainer ValidPatterns;
    TArray<FGameplayTag> InputArray;
    PatternTags.GetGameplayTagArray(InputArray);

    for (const FGameplayTag& Tag : InputArray)
    {
        if (DistancePatterns.HasTag(Tag))
        {
            ValidPatterns.AddTag(Tag);
        }
    }

    // 캐릭터 타입 필터링
    ValidPatterns = PatternData->FilterPatternsByCharacterType(ValidPatterns, IsBattleMage);

    if (ValidPatterns.IsEmpty())
    {
        // 폴백: 거리 무시하고 캐릭터 타입만 맞추기
        return SelectRandomPatternFromTags(PatternTags);
    }

    // 최근 사용한 패턴 제외
    FGameplayTagContainer FilteredPatterns;
    TArray<FGameplayTag> TagArray;
    ValidPatterns.GetGameplayTagArray(TagArray);

    for (const FGameplayTag& Tag : TagArray)
    {
        if (!RecentlyUsedPatterns.HasTag(Tag))
        {
            FilteredPatterns.AddTag(Tag);
        }
    }

    if (FilteredPatterns.IsEmpty())
    {
        FilteredPatterns = ValidPatterns;
    }

    // 랜덤 선택
    TArray<FGameplayTag> FinalArray;
    FilteredPatterns.GetGameplayTagArray(FinalArray);
    
    int32 RandomIndex = FMath::RandRange(0, FinalArray.Num() - 1);
    FGameplayTag SelectedTag = FinalArray[RandomIndex];

    RecentlyUsedPatterns.AddTag(SelectedTag);
    
    // 최대 개수 제한
    TArray<FGameplayTag> RecentArray;
    RecentlyUsedPatterns.GetGameplayTagArray(RecentArray);
    if (RecentArray.Num() > MaxRecentPatternMemory)
    {
        RecentlyUsedPatterns.RemoveTag(RecentArray[0]);
    }

    return PatternData->GetActionPatternForTag(SelectedTag);
}