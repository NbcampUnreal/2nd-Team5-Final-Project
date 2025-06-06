// SLCompanionCharacter.cpp
#include "SLCompanionCharacter.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AnimInstances/SLAICharacterAnimInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/SLAIFunctionLibrary.h"
#include "AI/SLCompanionPatternData.h"
#include "AI/Components/SLCompanionFlyingComponent.h"
#include "AI/GamePlayTag/AIGamePlayTag.h"
#include "AI/Projectile/SLAIProjectile.h"
#include "AI/Projectile/SLHomingProjectile.h"
#include "AI/SLMonster/SLMonster.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controller/SLBaseAIController.h"
#include "Controller/SLCompanionNormalAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Damage.h"

FSLCompanionActionPatternMappingRow::FSLCompanionActionPatternMappingRow()
{
    ActionPattern = ECompanionActionPattern::ECAP_None;
    DistanceType = ECompanionDistanceType::ECDT_Melee;
    bIsBattleMagePattern = true;
    Priority = 0;
    bIsLoop = false;
}

FNiagaraSpawnParams::FNiagaraSpawnParams()
{
    NiagaraSystem = nullptr;
    Location = FVector::ZeroVector;
    Rotation = FRotator::ZeroRotator;
    Scale = FVector(1.0f);
    bAutoDestroy = true;
    bAutoActivate = true;
    PoolingMethod = ENCPoolMethod::None;  
    bPreCullCheck = true;
    EffectDuration = 3.0f;
}

ASLCompanionCharacter::ASLCompanionCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    
    bCanBeExecuted = false;
    IsHitReaction = true;
    
    ProjectileSocketName = "hand_r";
    bIsInCombat = false;
    IsBattleMage = false;

    MeleeRangeThreshold = 200.0f;
    MidRangeThreshold = 500.0f;
    LongRangeThreshold = 800.0f;
    
    MaxRecentPatternMemory = 3;
    PatternData = nullptr;
    CurrentActionPattern = ECompanionActionPattern::ECAP_None;
    bIsTeleporting = false;

    FlyingComponent = CreateDefaultSubobject<USLCompanionFlyingComponent>(TEXT("FlyingComponent"));
    GetCharacterMovement()->NavAgentProps.bCanFly = true;
    GetCharacterMovement()->bCanWalkOffLedges = true;
    GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
}

void ASLCompanionCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (FlyingComponent)
    {
        
        FlyingComponent->OnFlyingStateChanged.AddDynamic(this, &ASLCompanionCharacter::OnFlyingStateChanged);
    }
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

        ASLCompanionNormalAIController* CompanionAIController = Cast<ASLCompanionNormalAIController>(AIController);
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

bool ASLCompanionCharacter::GetIsBattleMage()
{
    return IsBattleMage;
}

void ASLCompanionCharacter::SetIsBattleMage(bool bInBattleMage)
{
    IsBattleMage = bInBattleMage;
}

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

    // 선택된 패턴의 Loop 상태 확인 및 업데이트
    if (PatternData->IsPatternLoop(SelectedTag))
    {
        SetIsLoop(true);
    }
    else
    {
        SetIsLoop(false);
    }

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

    // 선택된 패턴의 Loop 상태 확인 및 업데이트
    if (PatternData->IsPatternLoop(SelectedTag))
    {
        SetIsLoop(true);
    }
    else
    {
        SetIsLoop(false);
    }

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

void ASLCompanionCharacter::SetCurrentActionPattern(ECompanionActionPattern NewPattern)
{
    CurrentActionPattern = NewPattern;

    if (AIController)
    {
        if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
        {
            BlackboardComponent->SetValueAsEnum(FName("CompanionPattern"), static_cast<uint8>(NewPattern));
        }
    }
}

bool ASLCompanionCharacter::GetIsTeleporting() const
{
    return bIsTeleporting;
}

void ASLCompanionCharacter::SetIsTeleporting(bool NewIsTeleporting)
{
    bIsTeleporting = NewIsTeleporting;
}

void ASLCompanionCharacter::PerformGroundExplosion(
    const FNiagaraSpawnParams& InWarningParams,
    const FNiagaraSpawnParams& InExplosionParams,
    float ExplosionRadius,
    EAttackAnimType AttackAnimType,
    float WarningDuration,
    int32 HitCount,
    float HitInterval)
{
    if (!GetWorld())
    {
        return;
    }

    FNiagaraSpawnParams WarningParams = InWarningParams;
    FNiagaraSpawnParams ExplosionParams = InExplosionParams;

    // 지면 위치 찾기
    WarningParams.Location = USLAIFunctionLibrary::FindGroundLocation(
        GetWorld(), 
        ExplosionParams.Location, 
        2000.0f, 
        IsDebugMode 
    ) + WarningParams.Offset;
    
    ExplosionParams.Location = USLAIFunctionLibrary::FindGroundLocation(
        GetWorld(), 
        ExplosionParams.Location, 
        2000.0f, 
        IsDebugMode 
    ) + ExplosionParams.Offset;
    
    auto ExecuteExplosion = [this, ExplosionParams, ExplosionRadius, AttackAnimType, HitCount, HitInterval]()
    {
        // 폭발 이펙트 재생
        SpawnNiagaraEffect(ExplosionParams);
        
        if (HitCount <= 1)
        {
            // 단일 히트
            ApplyExplosionDamage(ExplosionParams.Location, ExplosionRadius, AttackAnimType, true);
        }
        else
        {
            // 다단 히트 설정
            FMultiHitData MultiHitData;
            MultiHitData.Location = ExplosionParams.Location;
            MultiHitData.Radius = ExplosionRadius;
            MultiHitData.AttackType = AttackAnimType;
            MultiHitData.RemainingHits = HitCount;
            MultiHitData.HitInterval = HitInterval;
            
            ProcessMultiHit(MultiHitData);
        }
    };
    
    // 경고가 있으면 지연 실행, 없으면 즉시 실행
    if (WarningParams.NiagaraSystem)
    {
        SpawnNiagaraEffect(WarningParams);
        
        FTimerHandle ExplosionTimer;
        GetWorld()->GetTimerManager().SetTimer(
            ExplosionTimer, 
            ExecuteExplosion, 
            WarningDuration, 
            false
        );
    }
    else
    {
        ExecuteExplosion();
    }
}

void ASLCompanionCharacter::CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType)
{
    if (!IsInvincibility)
    {
        CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

        if (DamageCauser)
        {
            UAISense_Damage::ReportDamageEvent(
                GetWorld(),
                this,           // 데미지 받은 액터
                DamageCauser,   // 데미지 준 액터
                DamageAmount,   // 데미지 양
                GetActorLocation(), // 데미지 받은 위치
                HitResult.Location  // 히트 위치
            );
        }

        if (CurrentHealth <= 0.0f)
        {
            if (!IsDead)
            {
                IsDead = true;
        	
                // 충돌 비활성화
                GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); 

                // 이동 중지
                GetCharacterMovement()->StopMovementImmediately();
                GetCharacterMovement()->DisableMovement();
            
                // AI 컨트롤러 중지
                if (AIController)
                {
                    if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
                    {
                        BlackboardComponent->SetValueAsBool(FName("Isdead"), true);
                    }
                }
            }
        }
    }
    
    if (DamageCauser && ShouldPlayHitReaction(DamageAmount) && !IsDead)
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
        HitDirection = HitDir;
        HitDirectionVector = LocalHitDirection;
        bIsHit = true;
    }

    if (HitEffectComponent && DamageCauser && !IsDead)
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

void ASLCompanionCharacter::ApplyExplosionDamage(FVector ExplosionLocation, float ExplosionRadius, EAttackAnimType AttackAnimType, bool bIsFirstHit)
{
    if (!GetWorld())
    {
        return;
    }
    
    TArray<FHitResult> HitResults;
    FCollisionShape ExplosionShape = FCollisionShape::MakeSphere(ExplosionRadius);
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    
    GetWorld()->SweepMultiByChannel(
        HitResults,
        ExplosionLocation,
        ExplosionLocation + FVector(0, 0, 1),
        FQuat::Identity,
        ECC_GameTraceChannel1,
        ExplosionShape,
        QueryParams
    );
    
    // 히트된 액터들 처리
    TSet<AActor*> HitActors;
    for (const FHitResult& Hit : HitResults)
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            // 중복 히트 방지 (같은 프레임 내)
            if (!HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);
                
                if (ASLAIBaseCharacter* OwnerCharacter = Cast<ASLAIBaseCharacter>(GetInstigator()))
                {
                    OwnerCharacter->GetBattleComponent()->SendHitResult(HitActor, Hit, AttackAnimType);
                }
            }
        }
    }
    
    // 디버그 표시
    if (IsDebugMode)
    {
        FColor DebugColor = bIsFirstHit ? FColor::Red : FColor::Orange;
        DrawDebugSphere(
            GetWorld(),
            ExplosionLocation,
            ExplosionRadius,
            32,
            DebugColor,
            false,
            0.5f
        );
        
        // 히트 카운트 표시
        if (!bIsFirstHit)
        {
            DrawDebugString(
                GetWorld(),
                ExplosionLocation + FVector(0, 0, ExplosionRadius),
                TEXT("Multi-Hit!"),
                nullptr,
                FColor::Yellow,
                0.5f
            );
        }
    }
}

UNiagaraComponent* ASLCompanionCharacter::SpawnNiagaraEffect(const FNiagaraSpawnParams& SpawnParams)
{
    if (!GetWorld() || !SpawnParams.NiagaraSystem)
    {
        return nullptr;
    }
    
    UNiagaraComponent* SpawnedComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        SpawnParams.NiagaraSystem,
        SpawnParams.Location,
        SpawnParams.Rotation,
        SpawnParams.Scale,
        SpawnParams.bAutoDestroy,
        SpawnParams.bAutoActivate,
        SpawnParams.PoolingMethod,
        SpawnParams.bPreCullCheck
    );
    
    if (SpawnedComponent)
    {
        //사용자 파라미터 설정
        SpawnedComponent->SetVariableFloat(FName("_ColorHue"), SpawnParams.UserParams._ColorHue);
        SpawnedComponent->SetVariableFloat(FName("_Size"), SpawnParams.UserParams._Size);
        SpawnedComponent->SetVariableFloat(FName("Z_Threashold"), SpawnParams.UserParams.Z_Threashold);
        
        // 이펙트 제거 타이머 설정
        if (SpawnParams.EffectDuration > 0.0f)
        {
            if (SpawnParams.PoolingMethod == ENCPoolMethod::None)
            {
                // 풀링을 사용하지 않는 경우에만 DestroyComponent 사용
                FTimerHandle RemovalTimer;
                GetWorld()->GetTimerManager().SetTimer(RemovalTimer, 
                    [SpawnedComponent]()
                    {
                        if (IsValid(SpawnedComponent))
                        {
                            SpawnedComponent->DestroyComponent();
                        }
                    }, 
                    SpawnParams.EffectDuration, false);
            }
            else
            {
                // 풀링을 사용하는 경우 Deactivate만 수행
                FTimerHandle RemovalTimer;
                GetWorld()->GetTimerManager().SetTimer(RemovalTimer, 
                    [SpawnedComponent]()
                    {
                        if (IsValid(SpawnedComponent))
                        {
                            SpawnedComponent->Deactivate();
                        }
                    }, 
                    SpawnParams.EffectDuration, false);
            }
        }
    }
    
    return SpawnedComponent;
}

void ASLCompanionCharacter::OnFlyingStateChanged(bool bIsFlying)
{
    
}

void ASLCompanionCharacter::ProcessMultiHit(const FMultiHitData& MultiHitData)
{
    // 첫 히트 즉시 적용
    ApplyExplosionDamage(MultiHitData.Location, MultiHitData.Radius, 
        MultiHitData.AttackType, true);
    
    if (MultiHitData.RemainingHits <= 1)
    {
        return;
    }
    
    // 나머지 히트 처리
    FTimerHandle MultiHitTimer;
    FMultiHitData UpdatedData = MultiHitData;
    UpdatedData.RemainingHits--;
    
    ActiveMultiHits.Add(MultiHitTimer, UpdatedData);
    
    GetWorld()->GetTimerManager().SetTimer(
        MultiHitTimer,
        [this, MultiHitTimer]() mutable  // mutable 추가
        {
            if (FMultiHitData* Data = ActiveMultiHits.Find(MultiHitTimer))
            {
                ApplyExplosionDamage(Data->Location, Data->Radius, 
                    Data->AttackType, false);
                Data->RemainingHits--;
                
                if (Data->RemainingHits <= 0)
                {
                    GetWorld()->GetTimerManager().ClearTimer(MultiHitTimer);
                    ActiveMultiHits.Remove(MultiHitTimer);
                }
            }
        },
        MultiHitData.HitInterval,
        true
    );
}

void ASLCompanionCharacter::BeginDestroy()
{
    for (auto& Pair : ActiveMultiHits)
    {
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(Pair.Key);
        }
    }
    ActiveMultiHits.Empty();
    
    Super::BeginDestroy();
}