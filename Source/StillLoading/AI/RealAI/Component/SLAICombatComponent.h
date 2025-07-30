#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLAICombatComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(SLAICombatComponent, Log, All);

class USLAIStateComponent;
class ASLBattleManager;

UENUM(BlueprintType)
enum class EAIAnimType : uint8
{
    Attack        UMETA(DisplayName = "공격"),
    Wonder        UMETA(DisplayName = "대기"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformAnim, EAIAnimType, AnimType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAICombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    USLAICombatComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 적 탐지 및 관리
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    AActor* FindEnemyInDetectionRange() const;
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void HandleEnemyDetection(AActor* DetectedEnemy);
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void HandleNoEnemyDetected();

    // 타겟 관리
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void SetTarget(AActor* NewTarget);
    UFUNCTION(BlueprintPure, Category = "AI|Combat")
    AActor* GetCurrentTarget() const { return CurrentTarget; }
    void ClearTarget();

    // 공격 관리
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    bool CanAttack() const;
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void PerformAttack();
    void UpdateAttacking(float DeltaTime);

    // 교전 권한 관리
    bool RequestEngagementPermission(AActor* TargetActor);
    void ReleaseCurrentTargetEngagement();

    // 적 추적 정보 (지원 모드에서 사용)
    FVector GetEnemyMovementDirection() const { return EnemyVelocity; }
    FVector GetLastEnemyPosition() const { return LastEnemyPosition; }

    // 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float DetectionRange = 800.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackRange = 150.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackCooldown = 2.0f;

    // 이벤트
    UPROPERTY(BlueprintAssignable, Category = "AI|Events")
    FOnTargetChanged OnTargetChanged;
    UPROPERTY(BlueprintAssignable, Category = "AI|Events")
    FOnPerformAnim OnPerformAnim;

private:
    UPROPERTY()
    TObjectPtr<USLAIStateComponent> StateComponent;
    UPROPERTY()
    TObjectPtr<AActor> CurrentTarget;

    // 적 추적
    FVector LastEnemyPosition = FVector::ZeroVector;
    FVector EnemyVelocity = FVector::ZeroVector;
    float LastEnemyTrackTime = 0.0f;
    
    // 공격 관련
    float LastAttackTime = -9999.0f;
    
    // 타겟 클리어 관련
    FTimerHandle TargetClearTimerHandle;
    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float TargetLostGracePeriod = 3.0f;

    // 내부 함수들
    bool IsValidEnemy(AActor* Actor) const;
    void LookAtTarget(AActor* Target);
    void UpdateEnemyTracking(AActor* Enemy);
    
    UFUNCTION()
    void ClearTargetInternal();
    void LogCombatModeStatus(const FString& Message) const;
};
