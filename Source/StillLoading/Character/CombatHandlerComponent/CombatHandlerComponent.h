#pragma once

#include "CoreMinimal.h"
#include "Character/DataAsset/AttackComboDataAsset.h"
#include "Components/ActorComponent.h"
#include "CombatHandlerComponent.generated.h"

class ASLPlayerCharacter;
class AChargingWidgetActor;

UENUM(BlueprintType)
enum class ECharacterComboState : uint8
{
    CCS_Normal UMETA(DisplayName = "Normal"),
    CCS_Empowered UMETA(DisplayName = "Empowered"),
};

/**
 * 전투 처리 컴포넌트
 * - 콤보 공격 처리
 * - 충전 상태 관리
 * - ChargingWidgetActor 관리
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API UCombatHandlerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatHandlerComponent();

    /** 충전 시작 */
    UFUNCTION(BlueprintCallable, Category = "Charging")
    void StartCharging();

    /** 충전 취소 */
    UFUNCTION(BlueprintCallable, Category = "Charging")
    void CancelCharging();

    /** 현재 콤보 정보 조회 */
    UFUNCTION()
    bool GetCurrentComboInfo(UAnimMontage*& OutMontage, FName& OutSectionName);

    /** 다음 콤보 정보 조회 */
    UFUNCTION()
    bool GetNextComboInfo(UAnimMontage*& OutMontage, FName& OutSectionName) const;

    /** 콤보 단계 진행 */
    UFUNCTION()
    void AdvanceCombo();

    /** 콤보 상태 초기화 */
    UFUNCTION()
    FORCEINLINE void ResetCombo() { CurrentComboIndex = 0; }

    /** 총 콤보 개수 반환 */
    UFUNCTION()
    FORCEINLINE int32 GetComboCount() const
    {
        return ComboDataAsset ? ComboDataAsset->ComboChain.Num() : 0;
    }

    /** Empowered 상태 설정 */
    UFUNCTION()
    void SetEmpoweredCombatMode(const ECharacterComboState Mode, float Duration = 7.0f);
    UFUNCTION()
    FORCEINLINE void SetCombatMode(const ECharacterComboState Mode) { CurrentMode = Mode; }

    /** 현재 Empowered 상태인지 반환 */
    UFUNCTION()
    FORCEINLINE bool IsEmpowered() const { return CurrentMode == ECharacterComboState::CCS_Empowered; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Empowered", meta = (ClampMin = "0.1", ClampMax = "30.0"))
    float MaxEmpoweredDuration = 30.f;

protected:
    virtual void BeginPlay() override;

private:
    /** 충전 완료 처리 */
    UFUNCTION()
    void FinishCharging();

    /** 충전 진행 상태 업데이트 */
    UFUNCTION()
    void UpdateChargingProgress();

    /** ChargingWidgetActor 생성 */
    UFUNCTION()
    void GenerateChargingWidget();

    /** 현재 활성 콤보 데이터 조회 */
    UFUNCTION()
    void GetActiveComboDataAsset(UAttackComboDataAsset*& DataAsset);

    /** 콤보 리셋 */
    void ResetCombatMode();

    /** -------------------- 콤보 -------------------- */

    /** 콤보 인덱스 */
    UPROPERTY()
    int32 CurrentComboIndex = 0;

    /** 콤보 모드 (Normal / Empowered) */
    UPROPERTY()
    ECharacterComboState CurrentMode = ECharacterComboState::CCS_Normal;

    /** -------------------- 충전 -------------------- */

    /** 충전 상태 */
    bool bIsCharging = false;

    /** 충전 경과 시간 */
    float ChargingElapsed = 0.f;

    /** TimerHandles */
    FTimerHandle ChargingFinishTimerHandle;
    FTimerHandle ChargingUpdateTimerHandle;
    FTimerHandle CombatModeResetTimer;

    /** -------------------- Character -------------------- */

    UPROPERTY()
    TObjectPtr<ASLPlayerCharacter> OwnerCharacter;

public:
    /** 일반 콤보 데이터 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    TObjectPtr<UAttackComboDataAsset> ComboDataAsset;

    /** Empowered 콤보 데이터 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    TObjectPtr<UAttackComboDataAsset> EmpoweredComboDataAsset;

    /** Air Attack 콤보 데이터 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    TObjectPtr<UAttackComboDataAsset> AirComboDataAsset;

    /** 충전 유지 시간 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charging", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float ChargingDuration = 0.4f;

    /** Charging Widget Actor Blueprint Class */
    UPROPERTY(EditDefaultsOnly, Category = "Charging")
    TSubclassOf<AChargingWidgetActor> ChargingWidgetActorClass;

private:
    /** 현재 생성된 Charging Widget Actor */
    UPROPERTY()
    AChargingWidgetActor* ChargingWidgetActor;
};
