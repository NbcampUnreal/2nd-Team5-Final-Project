#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLAISupportModeComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAISupportComponent, Log, All);

class USLAIStateComponent;
class USLAICombatComponent;

UENUM(BlueprintType)
enum class EAISupportState : uint8
{
    None                    UMETA(DisplayName = "None"),
    FindingSafePosition     UMETA(DisplayName = "Finding Safe Position"),
    MovingBackward          UMETA(DisplayName = "Moving Backward"),
    WaitingAndRotating      UMETA(DisplayName = "Waiting And Rotating"),
    MovingForward           UMETA(DisplayName = "Moving Forward"),
    Completed               UMETA(DisplayName = "Completed")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAISupportModeComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    USLAISupportModeComponent();
    virtual void BeginPlay() override;

    // 지원 모드 시작/종료
    UFUNCTION(BlueprintCallable, Category = "AI|Support")
    void StartSupportMode();
    
    UFUNCTION(BlueprintCallable, Category = "AI|Support")
    void CompleteSupportMode();
    
    // 지원 모드 업데이트
    void UpdateSupportMode(float DeltaTime);
    
    // 상태 관리
    UFUNCTION(BlueprintPure, Category = "AI|Support")
    EAISupportState GetCurrentSupportState() const { return CurrentSupportState; }

    // 접근 방향 업데이트 (StateComponent에서 호출)
    void UpdateApproachDirection(const FVector& TargetLocation);

private:
    // 컴포넌트 참조들
    UPROPERTY()
    TObjectPtr<USLAIStateComponent> StateComponent;
    
    UPROPERTY()
    TObjectPtr<USLAICombatComponent> CombatComponent;

    // 지원모드 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Support", meta = (AllowPrivateAccess = "true"))
    EAISupportState CurrentSupportState = EAISupportState::None;

    // 이동 관련
    FVector SupportBackwardPosition = FVector::ZeroVector;
    FVector SupportForwardPosition = FVector::ZeroVector;
    FRotator SupportTargetRotation = FRotator::ZeroRotator;
    float SupportMoveDistance = 110.0f;
    
    // 타이머 관련
    FTimerHandle SupportWaitTimerHandle;
    float SupportWaitDuration = 0.0f;
    
    // 접근 방향 추적
    FVector LastApproachDirection = FVector::ZeroVector;
    
    // 충돌 체크 캐싱
    mutable float LastCollisionCheckTime = 0.0f;
    mutable bool bLastCollisionCheckResult = false;
    static constexpr float COLLISION_CHECK_INTERVAL = 0.2f;

    // 상태 관리
    void SetSupportState(EAISupportState NewState);
    void OnEnterSupportState(EAISupportState NewState);
    
    // 후진 위치 찾기 (메인 함수들)
    void FindSafeBackwardPosition();
    void CalculateRetreatDirection(FVector& OutRetreatDirection);
    void GenerateRetreatCandidates(const FVector& OriginLocation, const FVector& RetreatDirection, TArray<FVector>& OutCandidates);
    
    // 방향 계산 헬퍼 함수들
    FVector GetEnemyMovementDirection() const;
    FVector GetApproachDirection() const;
    FVector CombineDirections(const FVector& EnemyDir, const FVector& ApproachDir) const;
    float CalculateEnemyWeight(const FVector& EnemyVelocity) const;
    FVector GetDefaultRetreatDirection() const;
    
    // 위치 검증 관련
    void FindSafePositionAsync(const TArray<FVector>& Candidates, int32 CurrentIndex);
    bool IsPositionSafe(const FVector& Position, float CheckRadius = 150.0f);
    bool CheckCollisionAtPosition(const FVector& Position, float CheckRadius);
    bool ShouldUseCachedResult() const;
    void UpdateCollisionCache(bool bResult) const;
    
    // 이동 실행
    void StartBackwardMovement();
    void StartForwardMovement();
    void AcceptBackwardPosition(const FVector& Position);
    void UseDefaultBackwardPosition();
    
    // 회전 및 대기
    void StartWaitingAndRotating();
    void SetRotationTarget();
    void StartWaitTimer();
    
    // 타이머 콜백
    UFUNCTION()
    void OnSupportWaitCompleted();
    
    // 유틸리티
    bool HasReachedTarget(const FVector& TargetPosition, float Threshold = 50.0f) const;
    void MoveToLocation(const FVector& Location);
    void StopMovement();
    void CalculateForwardPosition();
    float CalculateAngleOffset(int32 Index, int32 TotalCount, float SpreadAngle) const;
    FVector RotateDirection(const FVector& Direction, float AngleDegrees) const;
    float CalculateVariedDistance() const;
    void LogSupportModeStatus(const FString& Message) const;
};
