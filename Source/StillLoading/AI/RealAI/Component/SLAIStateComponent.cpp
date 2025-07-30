#include "SLAIStateComponent.h"

#include "AIController.h"
#include "SLAICombatComponent.h"
#include "SLAISupportModeComponent.h"
#include "AI/RealAI/SLMonsterAICharacter.h"
#include "AI/RealAI/SLMonsterAICharacterBase.h"
#include "AI/RealAI/BattleManager/SLBattleManager.h"
#include "AI/RealAI/Spawner/SLSwarmSpawner.h"
#include "Character/GamePlayTag/GamePlayTag.h"
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogAIStateComponent);

USLAIStateComponent::USLAIStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;
	CurrentState = EAIBattleState::Idle;
	CurrentTargetPointIndex = 0;
}

void USLAIStateComponent::BeginPlay()
{
    Super::BeginPlay();

    CombatComponent = GetOwner()->FindComponentByClass<USLAICombatComponent>();
    SupportModeComponent = GetOwner()->FindComponentByClass<USLAISupportModeComponent>();
    
    if (!CombatComponent || !SupportModeComponent)
    {
        LogStateModeStatus(TEXT("필수 AI 컴포넌트들을 찾을 수 없습니다!"));
    }

    Initialize();
}

void USLAIStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!IsValid(GetOwner())) return;

    if (ASLMonsterAICharacter* MyCharacter = Cast<ASLMonsterAICharacter>(GetOwner()))
    {
        if (MyCharacter->IsInPrimaryState(TAG_AI_IsPlayingMontage)) return;
    }

    if (CombatComponent)
    {
        AActor* DetectedEnemy = CombatComponent->FindEnemyInDetectionRange();
        CombatComponent->HandleEnemyDetection(DetectedEnemy);
    }
    
    UpdateCurrentState(DeltaTime);
}

void USLAIStateComponent::UpdateCurrentState(float DeltaTime)
{
    switch (CurrentState)
    {
    case EAIBattleState::Idle:
    case EAIBattleState::Moving:
        break;
        
    case EAIBattleState::Attacking:
        if (CombatComponent)
        {
            CombatComponent->UpdateAttacking(DeltaTime);
        }
        break;

    case EAIBattleState::SupportOrIdle:
        if (SupportModeComponent)
        {
            SupportModeComponent->UpdateSupportMode(DeltaTime);
        }
        break;
        
    default:
        LogStateModeStatus(TEXT("알 수 없는 AI 상태"));
        break;
    }
}

void USLAIStateComponent::SetState(EAIBattleState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        OnEnterState(NewState);
        OnStateChanged.Broadcast(NewState);
        LogStateModeStatus(FString::Printf(TEXT("상태 변경 -> %s"), *UEnum::GetValueAsString(NewState)));
    }
}

void USLAIStateComponent::OnEnterState(EAIBattleState NewState)
{
    switch (NewState)
    {
    case EAIBattleState::Idle:
        if (CachedAIController.IsValid())
        {
            CachedAIController->StopMovement();
            CachedAIController->ClearFocus(EAIFocusPriority::Gameplay);
        }
        RequestNextTargetPoint();
        break;
        
    case EAIBattleState::Moving:
        break;
        
    case EAIBattleState::Attacking:
        if (CachedAIController.IsValid())
        {
            CachedAIController->StopMovement();
        }
        break;
        
    case EAIBattleState::SupportOrIdle:
        break;
    }
}

void USLAIStateComponent::Initialize()
{
    if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    {
        CachedAIController = Cast<AAIController>(OwnerPawn->GetController());
        if (CachedAIController.IsValid())
        {
            if (IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(CachedAIController))
            {
                MyTeamId = OwnerTeamAgent->GetGenericTeamId();
            }

            if (CachedAIController->GetPathFollowingComponent())
            {
                CachedAIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
                    this, &USLAIStateComponent::OnMoveCompleted);
            }
        }
    }
}

void USLAIStateComponent::ActivateAndMoveToInitialTarget(int32 InitialTargetPointIndex)
{
    if (CurrentState != EAIBattleState::Idle)
    {
        return;
    }

    SetCurrentTargetPointIndex(InitialTargetPointIndex);
    RequestNextTargetPoint();
}

void USLAIStateComponent::DeactivateAndReset()
{
    if (CachedAIController.IsValid())
    {
        CachedAIController->StopMovement();
    }

    if (CombatComponent)
    {
        CombatComponent->ReleaseCurrentTargetEngagement();
        CombatComponent->ClearTarget();
    }

    SetState(EAIBattleState::Idle);
    CurrentTargetPointIndex = 0;
}

void USLAIStateComponent::SetMovementTarget(FVector NewTargetLocation)
{
    if (NewTargetLocation.IsNearlyZero(KINDA_SMALL_NUMBER))
    {
        LogStateModeStatus(TEXT("유효하지 않은 이동 목표"));
        return;
    }

    MovementTargetLocation = NewTargetLocation;
    
    if (SupportModeComponent)
    {
        SupportModeComponent->UpdateApproachDirection(NewTargetLocation);
    }

    if (CachedAIController.IsValid())
    {
        FAIRequestID RequestID = CachedAIController->MoveToLocation(MovementTargetLocation);

        if (RequestID == FAIRequestID::InvalidRequest)
        {
            LogStateModeStatus(TEXT("MoveToLocation 실패"));
            SetState(EAIBattleState::Idle);
            return;
        }

        LogStateModeStatus(FString::Printf(TEXT("이동 시작: %s"), *MovementTargetLocation.ToString()));
    }
    else
    {
        LogStateModeStatus(TEXT("AI 컨트롤러가 유효하지 않음"));
        SetState(EAIBattleState::Idle);
    }
}

void USLAIStateComponent::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if (CurrentState == EAIBattleState::Moving)
    {
        GetWorld()->GetTimerManager().SetTimer(
            MovementCompletionTimerHandle,
            this,
            &USLAIStateComponent::RequestNextTargetPoint,
            0.1f,
            false
        );
    }
}

void USLAIStateComponent::RequestNextTargetPoint()
{
    const ASLMonsterAICharacterBase* MyCharacter = Cast<ASLMonsterAICharacterBase>(GetOwner());
    if (!MyCharacter) return;

    if (IsValid(MyCharacter->BattleManager) && IsValid(MyCharacter->BornSpawner))
    {
        const FVector NextLocation = MyCharacter->BattleManager->GetNextTargetPointLocationForSpawner(
            MyCharacter->BornSpawner, CurrentTargetPointIndex);

        if (!NextLocation.IsNearlyZero())
        {
            FVector CurrentLocation = GetOwner()->GetActorLocation();
            float DistanceToTarget = FVector::Dist(CurrentLocation, NextLocation);

            if (DistanceToTarget > 300.0f)
            {
                SetMovementTarget(NextLocation);
                SetState(EAIBattleState::Moving);
            }
            else
            {
                SetState(EAIBattleState::Idle);
            }
        }
        else
        {
            LogStateModeStatus(TEXT("더 이상 유효한 타겟 포인트 없음"));
            SetState(EAIBattleState::Idle);
        }
    }
    else
    {
        LogStateModeStatus(TEXT("BattleManager 또는 Spawner가 유효하지 않음"));
        SetState(EAIBattleState::Idle);
    }
}

void USLAIStateComponent::SetCurrentTargetPointIndex(int32 NewIndex)
{
    CurrentTargetPointIndex = NewIndex;
}

void USLAIStateComponent::LogStateModeStatus(const FString& Message) const
{
    UE_LOG(LogAIStateComponent, Log, TEXT("%s [상태모드]: %s"), 
           GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"), *Message);
}