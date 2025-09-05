#pragma once

#include "CoreMinimal.h"
#include "SLAIStateComponent.h"
#include "Components/ActorComponent.h"
#include "SLAICombatComponent.generated.h"

class ASLMonsterAICharacter;
DECLARE_LOG_CATEGORY_EXTERN(LogAICombatComponent, Log, All);

class USLAIStateComponent;
class ASLBattleManager;

USTRUCT(BlueprintType)
struct FTargetInfomation
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<AActor> TargetActor = nullptr;

    UPROPERTY()
    int32 HoldingTime = 0;
};

UENUM(BlueprintType)
enum class EAIAnimType : uint8
{
    Attack        UMETA(DisplayName = "공격"),
    Wonder        UMETA(DisplayName = "대기"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAICombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    USLAICombatComponent();
    virtual void BeginPlay() override;
    
    // 적 탐지 및 관리
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    AActor* FindEnemyInDetectionRange();
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void HandleEnemyDetection(AActor* DetectedEnemy);
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void HandleNoEnemyDetected();
    
    void UpdateFakeMovement(const FVector& TargetLocation, float DeltaTime);
    void UpdateFakeMovement(float DeltaTime);

    // 서포트 모드 관련
    void UpdateSupporting(float DeltaTime);
    FORCEINLINE bool IsSupporting() const { return bIsSupporting; }

    // 후퇴 관련
    void RetreatFromTarget();
    void StartRetreating();
    void StopRetreating();
    void TryScheduleRetreat();

    FTimerHandle BeginRetreatTimerHandle;
    FTimerHandle RetreatTimerHandle;

    // 공전 관련
    void StartRandomTurn();
    void FinishRandomTurn();
    void UpdateOrbiting();

    FTimerHandle OrbitUpdateTimerHandle;
    FTimerHandle EndOrbitTimerHandle;

    // 타겟 관리
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void SetTarget(AActor* NewTarget);
    UFUNCTION(BlueprintPure, Category = "AI|Combat")
    FORCEINLINE AActor* GetCurrentTarget() const { return CurrentTarget.TargetActor; }
    void ClearTarget();

    // 공격 관리
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    bool CanAttack() const;
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void PerformAttack(float DeltaTime);
    void UpdateAttacking(float DeltaTime);

    // 교전 권한 관리
    bool RequestEngagementPermission(AActor* TargetActor);
    void ReleaseCurrentTargetEngagement();
    void SafeLookAtTarget(AActor* Target, float DeltaTime);

    // 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float DetectionRange = 800.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackCooldown = 3.0f;

private:
    UPROPERTY()
    TObjectPtr<USLAIStateComponent> StateComponent;
    UPROPERTY()
    FTargetInfomation CurrentTarget;
    UPROPERTY()
    TObjectPtr<ASLMonsterAICharacter> CachedMyCharacter;
    UPROPERTY()
    TObjectPtr<AAIController> CachedAIController;

    // 공전용
    bool bIsOrbiting = false;
    float OrbitDirection = 1.0f;
    float DefaultBrakingDeceleration = 0.f;

    // 적 추적
    bool bIsRetreating = false;
    float TargetFoundTime = 0.f;
    float TargetAvailTime = 10.0f;

    // 후퇴 거리
    float RetreatDistance = 400.0f;
    
    // 공격 관련
    float LastAttackTime = -9999.0f;
    float OriginalSpeed = 0.f;

    // 서포트 모드 관련
    bool bIsSupporting;

    // 타이머
    FTimerHandle TargetClearTimerHandle;

    // 타겟 해제까지 걸리는 시간
    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float TargetLostGracePeriod = 3.0f;

    bool IsValidEnemy(AActor* Actor) const;
    
    UFUNCTION()
    void ClearTargetInternal();
    void LogCombatModeStatus(const FString& Message) const;
};
