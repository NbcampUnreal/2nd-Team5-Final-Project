#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "AI/RealAI/Task/BTTask_RetreatFromTarget.h"
#include "Navigation/PathFollowingComponent.h"
#include "SLAIStateComponent.generated.h"

class USLAILODComponent;
class ASLMonsterAICharacter;
class USLAICombatComponent;
class USLAISupportModeComponent;
class AAIController;
class ASLSwarmSpawner;

DECLARE_LOG_CATEGORY_EXTERN(LogAIStateComponent, Log, All);

UENUM(BlueprintType)
enum class EAIBattleState : uint8
{
    Idle            UMETA(DisplayName = "대기"),
    Moving          UMETA(DisplayName = "이동"),
    Attacking       UMETA(DisplayName = "공격"),
    FakeMoving,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIStateChanged, EAIBattleState, NewState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAIStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    USLAIStateComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // 상태 관리
    UFUNCTION(BlueprintCallable, Category = "AI|State")
    void SetState(EAIBattleState NewState);
    
    UFUNCTION(BlueprintPure, Category = "AI|State")
    FORCEINLINE EAIBattleState GetCurrentState() const { return CurrentState; }
    
    // 초기화 및 활성화
    void Initialize();
    void ActivateAndMoveToInitialTarget(int32 InitialTargetPointIndex);
    void DeactivateAndReset();

    // 이동 목표 설정
    UFUNCTION(BlueprintCallable, Category = "AI|Movement")
    void SetMovementTarget(FVector NewTargetLocation, float AvailRange = 150.f);

    // 순찰 관리
    UFUNCTION(BlueprintCallable, Category = "AI|Movement")
    void RequestNextTargetPoint();
    void SetCurrentTargetPointIndex(int32 NewIndex);

    // 서포트 모드 관현
    UFUNCTION()
    void StartSupportMovement(AActor* TargetToSupport);

    // 컴포넌트 참조 가져오기
    UFUNCTION(BlueprintPure, Category = "AI|Components")
    USLAICombatComponent* GetCombatComponent() const { return CombatComponent; }

    // 이벤트
    UPROPERTY(BlueprintAssignable, Category = "AI|Events")
    FOnAIStateChanged OnStateChanged;

    // 공개 참조들 (다른 컴포넌트에서 사용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Team")
    FGenericTeamId MyTeamId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
    EAIBattleState CurrentState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Controller")
    TWeakObjectPtr<AAIController> CachedAIController;

    UPROPERTY()
    TObjectPtr<ASLMonsterAICharacter> CachedMyCharacter;

    // 이동 관련 (다른 컴포넌트에서 참조)
    FVector MovementTargetLocation;

    // LOD 적용
    FTimerHandle DetectionTimerHandle;

private:
    // 로그 관리
    void LogStateModeStatus(const FString& Message) const;
    
    // 상태 관리
    void OnEnterState(EAIBattleState NewState);
    void UpdateCurrentState(float DeltaTime);
    
    // 이동 완료 콜백
    void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
    FTimerHandle MovementCompletionTimerHandle;

    // 컴포넌트 참조들
    UPROPERTY()
    TObjectPtr<USLAICombatComponent> CombatComponent;
    UPROPERTY()
    TObjectPtr<USLAILODComponent> CachedLODComponent;
    
    // 순찰 관련
    int32 CurrentTargetPointIndex = 0;

    // 서포트 모드 관련
    FVector FindSupportPosition(const FVector& TargetLocation, const FVector& MyLocation) const;

    // 적 탐지 관련
    void PerformEnemyDetection();
    
    TWeakObjectPtr<AActor> LastDetectedEnemy;
    UPROPERTY()
    TObjectPtr<AActor> SupportTargetActor = nullptr;
    UPROPERTY()
    bool bIsSupportMoving = false;
};
