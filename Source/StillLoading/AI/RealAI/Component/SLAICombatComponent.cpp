
#include "SLAICombatComponent.h"

#include "AIController.h"
#include "SLAIStateComponent.h"
#include "SLAISupportModeComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "Engine/OverlapResult.h"

DEFINE_LOG_CATEGORY(SLAICombatComponent);

USLAICombatComponent::USLAICombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.TickInterval = 0.5f;
    LastAttackTime = -9999.0f;
}

void USLAICombatComponent::BeginPlay()
{
    Super::BeginPlay();
    
    StateComponent = GetOwner()->FindComponentByClass<USLAIStateComponent>();
    if (!StateComponent)
    {
        LogCombatModeStatus(FString::Printf(TEXT("USLAIStateComponent를 찾을 수 없습니다!")));
    }
}

void USLAICombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (IsValid(CurrentTarget))
    {
        UpdateEnemyTracking(CurrentTarget);
    }
}

AActor* USLAICombatComponent::FindEnemyInDetectionRange() const
{
    TArray<FOverlapResult> OverlappingResults;
    const FVector OwnerLocation = GetOwner()->GetActorLocation();

    GetWorld()->OverlapMultiByChannel(
        OverlappingResults,
        OwnerLocation,
        FQuat::Identity,
        ECollisionChannel::ECC_Pawn,
        FCollisionShape::MakeSphere(DetectionRange),
        FCollisionQueryParams(SCENE_QUERY_STAT(AIAOD_Detection), false)
    );

    AActor* NearestEnemy = nullptr;
    float MinDistanceSq = FLT_MAX;

    for (const FOverlapResult& Overlap : OverlappingResults)
    {
        AActor* OverlappedActor = Overlap.GetActor();
        
        if (!IsValidEnemy(OverlappedActor))
            continue;

        const float DistanceSq = FVector::DistSquared(OwnerLocation, OverlappedActor->GetActorLocation());
        if (DistanceSq < MinDistanceSq)
        {
            MinDistanceSq = DistanceSq;
            NearestEnemy = OverlappedActor;
        }
    }

    return NearestEnemy;
}

void USLAICombatComponent::HandleEnemyDetection(AActor* DetectedEnemy)
{
    if (IsValid(DetectedEnemy))
    {
        if (CurrentTarget != DetectedEnemy)
        {
            if (IsValid(CurrentTarget))
            {
                ReleaseCurrentTargetEngagement();
            }
            
            // 새로운 적 발견 시 위치 추적 초기화
            LastEnemyPosition = DetectedEnemy->GetActorLocation();
            LastEnemyTrackTime = GetWorld()->GetTimeSeconds();
            EnemyVelocity = FVector::ZeroVector;
            
            if (RequestEngagementPermission(DetectedEnemy))
            {
                SetTarget(DetectedEnemy);
                if (StateComponent)
                {
                    StateComponent->SetState(EAIBattleState::Attacking);
                }
                
                // 틱 활성화 (적 추적용)
                SetComponentTickEnabled(true);
            }
            else
            {
                // 교전 권한 거부 시 임시로 타겟 설정 (지원 모드에서 방향 계산용)
                CurrentTarget = DetectedEnemy;

                if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
                {
                    MyCharacter->AISupportComp->StartSupportMode();
                }
                
                return;
            }
        }
        else
        {
            // 기존 타겟의 위치 업데이트
            UpdateEnemyTracking(DetectedEnemy);
        }
        
        if (GetWorld()->GetTimerManager().IsTimerActive(TargetClearTimerHandle))
        {
            GetWorld()->GetTimerManager().ClearTimer(TargetClearTimerHandle);
        }
    }
    else
    {
        HandleNoEnemyDetected();
    }
}

void USLAICombatComponent::HandleNoEnemyDetected()
{
    if (IsValid(CurrentTarget))
    {
        if (!GetWorld()->GetTimerManager().IsTimerActive(TargetClearTimerHandle))
        {
            LogCombatModeStatus(FString::Printf(TEXT("타겟 '%s' 탐지 범위 이탈 - %.1f초 후 타겟 해제"), 
                *CurrentTarget->GetName(), TargetLostGracePeriod));
            
            GetWorld()->GetTimerManager().SetTimer(
                TargetClearTimerHandle,
                this,
                &USLAICombatComponent::ClearTargetInternal,
                TargetLostGracePeriod,
                false
            );
        }
    }
    else if (StateComponent && StateComponent->GetCurrentState() == EAIBattleState::Attacking)
    {
        StateComponent->RequestNextTargetPoint();
    }
}

void USLAICombatComponent::SetTarget(AActor* NewTarget)
{
    if (CurrentTarget != NewTarget)
    {
        if (IsValid(CurrentTarget) && CurrentTarget != NewTarget)
        {
            ReleaseCurrentTargetEngagement();
        }
        
        CurrentTarget = NewTarget;
        OnTargetChanged.Broadcast(NewTarget);
        LogCombatModeStatus(FString::Printf(TEXT("타겟 설정 -> %s"), 
            IsValid(NewTarget) ? *NewTarget->GetName() : TEXT("없음")));
        
        // 타겟이 있으면 틱 활성화, 없으면 비활성화
        SetComponentTickEnabled(IsValid(NewTarget));
    }
}

void USLAICombatComponent::ClearTarget()
{
    SetTarget(nullptr);
    GetWorld()->GetTimerManager().ClearTimer(TargetClearTimerHandle);
}

bool USLAICombatComponent::CanAttack() const
{
    if (!IsValid(CurrentTarget)) return false;
    
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime < AttackCooldown)
    {
        return false;
    }

    float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget->GetActorLocation());
    return Distance <= AttackRange;
}

void USLAICombatComponent::PerformAttack()
{
    if (CanAttack())
    {
        LastAttackTime = GetWorld()->GetTimeSeconds();
        LogCombatModeStatus(FString::Printf(TEXT("%s 공격!"), *CurrentTarget->GetName()));

        ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner());
        if (MyCharacter)
        {
            MyCharacter->PlayAttackAnim();
            OnPerformAnim.Broadcast(EAIAnimType::Attack);
        }
    }
}

void USLAICombatComponent::UpdateAttacking(float DeltaTime)
{
    if (!IsValid(CurrentTarget))
    {
        if (StateComponent && StateComponent->CachedAIController.IsValid())
        {
            StateComponent->CachedAIController->ClearFocus(EAIFocusPriority::Gameplay);
        }
        
        ReleaseCurrentTargetEngagement();
        ClearTarget();
        
        if (StateComponent)
        {
            StateComponent->RequestNextTargetPoint();
        }
        return;
    }

    LookAtTarget(CurrentTarget);

    float Distance = FVector::Dist(GetOwner()->GetActorLocation(), CurrentTarget->GetActorLocation());

    if (Distance > AttackRange * 1.2f)
    {
        if (StateComponent)
        {
            StateComponent->SetMovementTarget(CurrentTarget->GetActorLocation());
        }
        return;
    }

    PerformAttack();
}

bool USLAICombatComponent::RequestEngagementPermission(AActor* TargetActor)
{
    if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
    {
        if (MyCharacter->BattleManager)
        {
            return MyCharacter->BattleManager->RequestEngagementPermission(GetOwner(), TargetActor);
        }
    }
    return false;
}

void USLAICombatComponent::ReleaseCurrentTargetEngagement()
{
    if (!IsValid(CurrentTarget)) return;
    
    if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
    {
        if (IsValid(MyCharacter->BattleManager))
        {
            MyCharacter->BattleManager->ReleaseEngagementPermission(GetOwner(), CurrentTarget);
            LogCombatModeStatus(FString::Printf(TEXT("타겟 %s와의 교전 권한 해제"), 
                *CurrentTarget->GetName()));
        }
    }
}

bool USLAICombatComponent::IsValidEnemy(AActor* Actor) const
{
    if (!IsValid(Actor) || Actor == GetOwner()) 
        return false;

    const APawn* TargetPawn = Cast<APawn>(Actor);
    if (!TargetPawn) 
        return false;

    AController* TargetController = TargetPawn->GetController();
    if (!TargetController) 
        return false;

    IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetController);
    if (!TargetTeamAgent) 
        return false;

    const FGenericTeamId TargetTeamId = TargetTeamAgent->GetGenericTeamId();
    if (TargetTeamId == FGenericTeamId::NoTeam) 
        return false;

    const ASLMonsterAICharacterBase* MyCharacter = Cast<ASLMonsterAICharacterBase>(GetOwner());
    if (!MyCharacter || !MyCharacter->BattleManager) 
        return false;

    bool bTargetIsPlayer = TargetController->IsPlayerController();
    return MyCharacter->BattleManager->AreEnemies(StateComponent->MyTeamId, TargetTeamId, bTargetIsPlayer);
}

void USLAICombatComponent::LookAtTarget(AActor* Target)
{
    if (!Target || !StateComponent || !StateComponent->CachedAIController.IsValid())
    {
        return;
    }

    StateComponent->CachedAIController->SetFocalPoint(Target->GetActorLocation());
}

void USLAICombatComponent::UpdateEnemyTracking(AActor* Enemy)
{
    if (!IsValid(Enemy)) return;
    
    FVector CurrentEnemyPos = Enemy->GetActorLocation();
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    if (!LastEnemyPosition.IsZero() && (CurrentTime - LastEnemyTrackTime) > 0.05f)
    {
        FVector MovementVector = CurrentEnemyPos - LastEnemyPosition;
        if (MovementVector.SizeSquared() > 25.0f) // 5유닛 이상 이동
        {
            EnemyVelocity = MovementVector.GetSafeNormal();
        }
    }
    
    LastEnemyPosition = CurrentEnemyPos;
    LastEnemyTrackTime = CurrentTime;
}

void USLAICombatComponent::ClearTargetInternal()
{
    if (!IsValid(CurrentTarget.Get()))
    {
        return;
    }

    LogCombatModeStatus(FString::Printf(TEXT("타겟이 탐지 범위에서 벗어난 후 %.1f초가 지나 타겟을 해제합니다."), 
        TargetLostGracePeriod));

    ReleaseCurrentTargetEngagement();
    
    ClearTarget();
    
    if (StateComponent)
    {
        StateComponent->RequestNextTargetPoint();
    }
}

void USLAICombatComponent::LogCombatModeStatus(const FString& Message) const
{
    UE_LOG(SLAICombatComponent, Log, TEXT("%s [전투모드]: %s"), 
           GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"), *Message);
}