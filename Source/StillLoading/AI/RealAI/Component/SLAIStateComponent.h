#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "AI/RealAI/Task/BTTask_RetreatFromTarget.h"
#include "Navigation/PathFollowingComponent.h"
#include "SLAIStateComponent.generated.h"

class ASLSwarmSpawner;
class AAIController;
class ASLBattleManager;

UENUM(BlueprintType)
enum class EAIBattleState : uint8
{
	Idle        UMETA(DisplayName = "대기"),
	Moving      UMETA(DisplayName = "이동"),
	Attacking   UMETA(DisplayName = "공격")
};

UENUM(BlueprintType)
enum class EAIAnimType : uint8
{
	Attack        UMETA(DisplayName = "공격"),
	Wonder        UMETA(DisplayName = "대기"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIStateChanged, EAIBattleState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAITargetChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformAnim, EAIAnimType, AnimType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAIStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
    USLAIStateComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Initialize();
	
	// 최초 이동 명령
	void ActivateAndMoveToInitialTarget(int32 InitialTargetPointIndex);

	// AI를 비활성화, 상태 초기화
	void DeactivateAndReset();

	// 다음 타겟 포인트를 요청하고 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "AIWar")
	void RequestNextTargetPoint();

	UFUNCTION(BlueprintCallable, Category = "AIWar")
    void SetState(EAIBattleState NewState);

    UFUNCTION(BlueprintCallable, Category = "AIWar")
    void SetTarget(AActor* NewTarget);

    // 이동 목표 설정
    UFUNCTION(BlueprintCallable, Category = "AIWar|Movement")
    void SetMovementTarget(FVector NewTargetLocation);

    UFUNCTION(BlueprintCallable, Category = "AIWar")
    void ClearTarget();

    UFUNCTION(BlueprintCallable, Category = "AIWar|Attack")
    bool CanAttack() const;

    UFUNCTION(BlueprintCallable, Category = "AIWar|Attack")
    void PerformAttack(); // 공격 로직은 여기서 직접 구현되거나 BlueprintNativeEvent로 연결될 수 있습니다.

	UFUNCTION()
	void UpdateAttacking(float DeltaTime);

    // 현재 타겟 포인트 인덱스 설정 (BattleManager에서 초기 설정)
    void SetCurrentTargetPointIndex(int32 NewIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIWar")
	FGenericTeamId MyTeamId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIWar|State")
	EAIBattleState CurrentState; // 이름 변경

	UPROPERTY(BlueprintAssignable, Category = "AIWar|Events")
	FOnAIStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "AIWar|Events")
	FOnAITargetChanged OnTargetChanged;

	UPROPERTY(BlueprintAssignable, Category = "AIWar|Events")
	FOnPerformAnim OnPerformAnim;
	
	// 감지 범위 (적 탐지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIWar|Detection")
	float DetectionRange = 1000.f;
	// 공격 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIWar|Attack")
	float AttackRange = 200.f;
	// 공격 쿨다운
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIWar|Attack")
	float AttackCooldown = 2.0f;

	// 현재 공격 대상
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIWar|Target")
	TWeakObjectPtr<AActor> CurrentTarget;
	
	UPROPERTY()
	TWeakObjectPtr<ASLSwarmSpawner> SourceSpawner; 

protected:
    virtual void BeginPlay() override;

private:
	// 이동 목표에 도달했을 때 호출될 함수
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	void OnEnterState(EAIBattleState NewState);
	
	UFUNCTION()
	void ClearTargetInternal();

	AActor* FindEnemyInDetectionRange() const;
	
	FTimerHandle MovementCompletionTimerHandle;
	FTimerHandle TargetClearTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "AI Combat")
	float TargetLostGracePeriod = 3.0f;
	
    TWeakObjectPtr<AAIController> CachedAIController;
    FVector MovementTargetLocation;
    float LastAttackTime = -9999.0f;

    UPROPERTY()
    int32 CurrentTargetPointIndex;
};
