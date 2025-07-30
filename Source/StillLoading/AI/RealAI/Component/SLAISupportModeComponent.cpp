#include "SLAISupportModeComponent.h"

#include "AIController.h"
#include "SLAICombatComponent.h"
#include "SLAIStateComponent.h"
#include "Engine/OverlapResult.h"

DEFINE_LOG_CATEGORY(LogAISupportComponent);

USLAISupportModeComponent::USLAISupportModeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLAISupportModeComponent::BeginPlay()
{
	Super::BeginPlay();

	StateComponent = GetOwner()->FindComponentByClass<USLAIStateComponent>();
	CombatComponent = GetOwner()->FindComponentByClass<USLAICombatComponent>();
    
	if (!StateComponent || !CombatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: 필수 컴포넌트들을 찾을 수 없습니다!"), *GetOwner()->GetName());
	}
}

void USLAISupportModeComponent::StartSupportMode()
{
    if (!StateComponent) return;
    
    StateComponent->SetState(EAIBattleState::SupportOrIdle);
    SetSupportState(EAISupportState::FindingSafePosition);
    
    SupportMoveDistance = FMath::RandRange(200.0f, 220.0f);
    SupportWaitDuration = FMath::RandRange(1.5f, 2.5f);
    
    LogSupportModeStatus(FString::Printf(TEXT("지원 모드 시작 - 거리: %.1f, 대기: %.1fs"), 
                                        SupportMoveDistance, SupportWaitDuration));
    
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
    {
        FindSafeBackwardPosition();
    });
}

void USLAISupportModeComponent::UpdateSupportMode(float DeltaTime)
{
    switch (CurrentSupportState)
    {
    case EAISupportState::FindingSafePosition:
        break;
        
    case EAISupportState::MovingBackward:
        if (HasReachedTarget(SupportBackwardPosition))
        {
            SetSupportState(EAISupportState::WaitingAndRotating);
        }
        break;
        
    case EAISupportState::WaitingAndRotating:
        break;
        
    case EAISupportState::MovingForward:
        if (HasReachedTarget(SupportForwardPosition))
        {
            SetSupportState(EAISupportState::Completed);
        }
        break;
        
    case EAISupportState::Completed:
        CompleteSupportMode();
        break;
        
    default:
        break;
    }
}

void USLAISupportModeComponent::CompleteSupportMode()
{
    SetSupportState(EAISupportState::None);
    
    if (StateComponent && StateComponent->CachedAIController.IsValid())
    {
        StateComponent->CachedAIController->ClearFocus(EAIFocusPriority::Gameplay);
    }
    
    // 지원 모드 완료 시 임시 타겟 해제
    if (CombatComponent && CombatComponent->GetCurrentTarget() && 
        StateComponent && StateComponent->GetCurrentState() == EAIBattleState::SupportOrIdle)
    {
        CombatComponent->SetTarget(nullptr);
    }
    
    if (StateComponent)
    {
        StateComponent->RequestNextTargetPoint();
    }
    
    LogSupportModeStatus("지원 모드 완료 - 순찰 재개");
}

void USLAISupportModeComponent::UpdateApproachDirection(const FVector& TargetLocation)
{
    if (GetOwner())
    {
        FVector CurrentLocation = GetOwner()->GetActorLocation();
        LastApproachDirection = (TargetLocation - CurrentLocation).GetSafeNormal();
        
        UE_LOG(LogTemp, VeryVerbose, TEXT("%s: 접근 방향 업데이트: %s"), 
               *GetOwner()->GetName(), *LastApproachDirection.ToString());
    }
}

void USLAISupportModeComponent::FindSafeBackwardPosition()
{
    if (!GetOwner())
    {
        LogSupportModeStatus("소유자가 유효하지 않음");
        return;
    }

    FVector OwnerLocation = GetOwner()->GetActorLocation();
    FVector RetreatDirection;
    
    // 1. 후진 방향 계산
    CalculateRetreatDirection(RetreatDirection);
    
    // 2. 후보 위치들 생성
    TArray<FVector> Candidates;
    GenerateRetreatCandidates(OwnerLocation, RetreatDirection, Candidates);
    
    // 3. 비동기로 안전한 위치 찾기
    FindSafePositionAsync(Candidates, 0);
}

void USLAISupportModeComponent::CalculateRetreatDirection(FVector& OutRetreatDirection)
{
    FVector EnemyDir = GetEnemyMovementDirection();
    FVector ApproachDir = GetApproachDirection();
    
    if (!EnemyDir.IsNearlyZero() && !ApproachDir.IsNearlyZero())
    {
        // 하이브리드 조합
        OutRetreatDirection = CombineDirections(EnemyDir, ApproachDir);
        LogSupportModeStatus("하이브리드 방향 계산 완료");
    }
    else if (!EnemyDir.IsNearlyZero())
    {
        // 적 이동 방향만 사용
        OutRetreatDirection = -EnemyDir;
        LogSupportModeStatus("적 이동 방향 기준");
    }
    else if (!ApproachDir.IsNearlyZero())
    {
        // 접근 방향만 사용
        OutRetreatDirection = -ApproachDir;
        LogSupportModeStatus("접근 방향 기준");
    }
    else
    {
        // 기본값: 적 반대 방향
        OutRetreatDirection = GetDefaultRetreatDirection();
        LogSupportModeStatus("기본 후진 방향 사용");
    }
}

void USLAISupportModeComponent::GenerateRetreatCandidates(const FVector& OriginLocation, const FVector& RetreatDirection, TArray<FVector>& OutCandidates)
{
    OutCandidates.Empty();
    
    const int32 NumCandidates = 8;
    const float SpreadAngle = 120.0f;
    
    for (int32 i = 0; i < NumCandidates; ++i)
    {
        // 부채꼴 형태로 후보 생성
        float AngleOffset = CalculateAngleOffset(i, NumCandidates, SpreadAngle);
        FVector Direction = RotateDirection(RetreatDirection, AngleOffset);
        float Distance = CalculateVariedDistance();
        
        FVector CandidatePos = OriginLocation + (Direction * Distance);
        OutCandidates.Add(CandidatePos);
        
        UE_LOG(LogTemp, VeryVerbose, TEXT("후보 %d: 각도%.1f, 거리%.1f"), 
               i, AngleOffset, Distance);
    }
}

FVector USLAISupportModeComponent::GetEnemyMovementDirection() const
{
    if (!CombatComponent)
    {
        return FVector::ZeroVector;
    }
    
    return CombatComponent->GetEnemyMovementDirection();
}

FVector USLAISupportModeComponent::GetApproachDirection() const
{
    if (!LastApproachDirection.IsZero())
    {
        return LastApproachDirection;
    }
    
    // StateComponent의 MovementTargetLocation에서 계산
    if (StateComponent && !StateComponent->MovementTargetLocation.IsZero())
    {
        FVector CurrentLocation = GetOwner()->GetActorLocation();
        return (StateComponent->MovementTargetLocation - CurrentLocation).GetSafeNormal();
    }
    
    return FVector::ZeroVector;
}

FVector USLAISupportModeComponent::CombineDirections(const FVector& EnemyDir, const FVector& ApproachDir) const
{
    float EnemyWeight = CalculateEnemyWeight(EnemyDir);
    float ApproachWeight = 1.0f - EnemyWeight;
    
    FVector EnemyRetreat = -EnemyDir;
    FVector ApproachRetreat = -ApproachDir;
    
    return (EnemyRetreat * EnemyWeight + ApproachRetreat * ApproachWeight).GetSafeNormal();
}

float USLAISupportModeComponent::CalculateEnemyWeight(const FVector& EnemyVelocity) const
{
    float EnemySpeed = EnemyVelocity.Size();
    
    if (EnemySpeed > 2.0f)
    {
        return 0.8f; // 빠른 이동 시 적 방향 우선
    }
    else if (EnemySpeed < 0.5f)
    {
        return 0.5f; // 느린 이동 시 균등
    }
    
    return 0.7f; // 기본값
}

FVector USLAISupportModeComponent::GetDefaultRetreatDirection() const
{
    // 적이 있으면 적 반대 방향, 없으면 캐릭터 뒤쪽
    if (CombatComponent && CombatComponent->GetCurrentTarget())
    {
        FVector OwnerPos = GetOwner()->GetActorLocation();
        FVector EnemyPos = CombatComponent->GetCurrentTarget()->GetActorLocation();
        return (OwnerPos - EnemyPos).GetSafeNormal();
    }
    
    return -GetOwner()->GetActorForwardVector();
}

void USLAISupportModeComponent::FindSafePositionAsync(const TArray<FVector>& Candidates, int32 CurrentIndex)
{
    if (CurrentIndex >= Candidates.Num())
    {
        // 모든 후보가 실패 - 기본 위치 사용
        UseDefaultBackwardPosition();
        return;
    }
    
    const FVector& Candidate = Candidates[CurrentIndex];
    
    if (IsPositionSafe(Candidate))
    {
        // 안전한 위치 발견
        AcceptBackwardPosition(Candidate);
        return;
    }
    
    // 다음 프레임에서 다음 후보 검사
    GetWorld()->GetTimerManager().SetTimerForNextTick([this, Candidates, CurrentIndex]()
    {
        FindSafePositionAsync(Candidates, CurrentIndex + 1);
    });
}

bool USLAISupportModeComponent::IsPositionSafe(const FVector& Position, float CheckRadius)
{
    // 성능 최적화: 캐싱된 결과 사용
    if (ShouldUseCachedResult())
    {
        return bLastCollisionCheckResult;
    }
    
    bool bResult = CheckCollisionAtPosition(Position, CheckRadius);
    UpdateCollisionCache(bResult);
    
    return bResult;
}

bool USLAISupportModeComponent::CheckCollisionAtPosition(const FVector& Position, float CheckRadius)
{
    TArray<FOverlapResult> OverlapResults;
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = false; // 성능 최적화
    
    bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        Position,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(CheckRadius),
        QueryParams
    );
    
    return !bHasOverlap; // 겹치지 않으면 안전
}

bool USLAISupportModeComponent::ShouldUseCachedResult() const
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    return (CurrentTime - LastCollisionCheckTime < COLLISION_CHECK_INTERVAL);
}

void USLAISupportModeComponent::UpdateCollisionCache(bool bResult) const
{
    LastCollisionCheckTime = GetWorld()->GetTimeSeconds();
    bLastCollisionCheckResult = bResult;
}

void USLAISupportModeComponent::AcceptBackwardPosition(const FVector& Position)
{
    SupportBackwardPosition = Position;
    StartBackwardMovement();
    
    LogSupportModeStatus(FString::Printf(TEXT("후진 위치 확정: %s"), *Position.ToString()));
}

void USLAISupportModeComponent::UseDefaultBackwardPosition()
{
    if (!GetOwner()) return;
    
    FVector OwnerLocation = GetOwner()->GetActorLocation();
    FVector DefaultDirection = GetDefaultRetreatDirection();
    
    SupportBackwardPosition = OwnerLocation + (DefaultDirection * SupportMoveDistance);
    StartBackwardMovement();
    
    LogSupportModeStatus("기본 후진 위치 사용");
}

void USLAISupportModeComponent::StartBackwardMovement()
{
    SetSupportState(EAISupportState::MovingBackward);
    MoveToLocation(SupportBackwardPosition);
    
    LogSupportModeStatus(FString::Printf(TEXT("후진 시작 - 거리: %.0f"), SupportMoveDistance));
}

void USLAISupportModeComponent::StartForwardMovement()
{
    CalculateForwardPosition();
    SetSupportState(EAISupportState::MovingForward);
    MoveToLocation(SupportForwardPosition);
    
    LogSupportModeStatus("전진 시작");
}

void USLAISupportModeComponent::StartWaitingAndRotating()
{
    StopMovement();
    SetRotationTarget();
    StartWaitTimer();
    
    LogSupportModeStatus(FString::Printf(TEXT("대기 및 회전 시작 - %.1f초"), SupportWaitDuration));
}

void USLAISupportModeComponent::SetSupportState(EAISupportState NewState)
{
    if (CurrentSupportState != NewState)
    {
        EAISupportState OldState = CurrentSupportState;
        CurrentSupportState = NewState;
        OnEnterSupportState(NewState);
    }
}

void USLAISupportModeComponent::OnEnterSupportState(EAISupportState NewState)
{
    switch (NewState)
    {
    case EAISupportState::WaitingAndRotating:
        StartWaitingAndRotating();
        break;
        
    case EAISupportState::MovingForward:
        StartForwardMovement();
        break;
        
    case EAISupportState::Completed:
        // CompleteSupportMode()에서 처리
        break;
        
    default:
        break;
    }
}

void USLAISupportModeComponent::SetRotationTarget()
{
    float RandomYaw = FMath::RandRange(-90.0f, 90.0f);
    FRotator CurrentRotation = GetOwner()->GetActorRotation();
    SupportTargetRotation = FRotator(0, CurrentRotation.Yaw + RandomYaw, 0);
}

void USLAISupportModeComponent::StartWaitTimer()
{
    GetWorld()->GetTimerManager().SetTimer(
        SupportWaitTimerHandle,
        this,
        &USLAISupportModeComponent::OnSupportWaitCompleted,
        SupportWaitDuration,
        false
    );
}

void USLAISupportModeComponent::OnSupportWaitCompleted()
{
    if (StateComponent && StateComponent->CachedAIController.IsValid())
    {
        FVector ForwardDirection = SupportTargetRotation.RotateVector(FVector::ForwardVector);
        FVector FocalPoint = GetOwner()->GetActorLocation() + (ForwardDirection * 1000.0f);
        StateComponent->CachedAIController->SetFocalPoint(FocalPoint);
    }
    
    SetSupportState(EAISupportState::MovingForward);
}

void USLAISupportModeComponent::CalculateForwardPosition()
{
    FVector CurrentLocation = GetOwner()->GetActorLocation();
    FVector ForwardDirection = SupportTargetRotation.RotateVector(FVector::ForwardVector);
    SupportForwardPosition = CurrentLocation + (ForwardDirection * (SupportMoveDistance * 0.5f)); // 절반 거리만 전진
}

bool USLAISupportModeComponent::HasReachedTarget(const FVector& TargetPosition, float Threshold) const
{
    if (!GetOwner()) return false;
    
    float Distance = FVector::Dist(GetOwner()->GetActorLocation(), TargetPosition);
    return Distance < Threshold;
}

void USLAISupportModeComponent::MoveToLocation(const FVector& Location)
{
    if (StateComponent && StateComponent->CachedAIController.IsValid())
    {
        StateComponent->CachedAIController->MoveToLocation(Location);
    }
}

void USLAISupportModeComponent::StopMovement()
{
    if (StateComponent && StateComponent->CachedAIController.IsValid())
    {
        StateComponent->CachedAIController->StopMovement();
    }
}

float USLAISupportModeComponent::CalculateAngleOffset(int32 Index, int32 TotalCount, float SpreadAngle) const
{
    return (SpreadAngle / (TotalCount - 1)) * Index - (SpreadAngle * 0.5f);
}

FVector USLAISupportModeComponent::RotateDirection(const FVector& Direction, float AngleDegrees) const
{
    return Direction.RotateAngleAxis(AngleDegrees, FVector::UpVector);
}

float USLAISupportModeComponent::CalculateVariedDistance() const
{
    float Variation = FMath::RandRange(0.9f, 1.1f);
    return SupportMoveDistance * Variation;
}

void USLAISupportModeComponent::LogSupportModeStatus(const FString& Message) const
{
    UE_LOG(LogAISupportComponent, Log, TEXT("%s [지원모드]: %s"), 
           GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"), *Message);
}