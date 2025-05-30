// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Character/SLAIBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "SLCompanionCharacter.generated.h"

// Companion의 행동 패턴
UENUM(BlueprintType)
enum class ECompanionActionPattern : uint8
{
    // Common
    ECAP_None UMETA(DisplayName = "None"),
	ECAP_Attack_Plunge UMETA(DisplayName = "Attack Plunge"), // 거리가 가까우면
	ECAP_Attack_Air UMETA(DisplayName = "Attack Air"),		// 거리가 멀면

    // BM Attack Patterns
	ECAP_BM_MeleeCombo1          UMETA(DisplayName = "BM Melee Combo 1"),      // BM 01->02->03->04->10
	ECAP_BM_MeleeCombo2          UMETA(DisplayName = "BM Melee Combo 2"),      // BM 13->05->06->08->12
	ECAP_BM_MeleeCombo3          UMETA(DisplayName = "BM Melee Combo 3"),      // BM 05->06->07->08->09
	
	ECAP_BM_QuickCombo1 UMETA(DisplayName = "BM Quick Combo 1"),      // BM 10->11->12
	ECAP_BM_HeavyCombo1 UMETA(DisplayName = "BM Heavy Combo 1"),      // BM 13->14->15->16
	ECAP_BM_FinisherCombo1 UMETA(DisplayName = "BM Finisher Combo 1"),// BM 17->18

    ECAP_BM_Attack01 UMETA(DisplayName = "BM Attack 01"),
    ECAP_BM_Attack02 UMETA(DisplayName = "BM Attack 02"),
    ECAP_BM_Attack03 UMETA(DisplayName = "BM Attack 03"),
    ECAP_BM_Attack04 UMETA(DisplayName = "BM Attack 04"),
    ECAP_BM_Attack05 UMETA(DisplayName = "BM Attack 05"),
    ECAP_BM_Attack06 UMETA(DisplayName = "BM Attack 06"),
    ECAP_BM_Attack07 UMETA(DisplayName = "BM Attack 07"),
    ECAP_BM_Attack08 UMETA(DisplayName = "BM Attack 08"),
    ECAP_BM_Attack09 UMETA(DisplayName = "BM Attack 09"),
    ECAP_BM_Attack10 UMETA(DisplayName = "BM Attack 10"),
    ECAP_BM_Attack11 UMETA(DisplayName = "BM Attack 11"),
    ECAP_BM_Attack12 UMETA(DisplayName = "BM Attack 12"),
    ECAP_BM_Attack13 UMETA(DisplayName = "BM Attack 13"),
    ECAP_BM_Attack14 UMETA(DisplayName = "BM Attack 14"),
    ECAP_BM_Attack15 UMETA(DisplayName = "BM Attack 15"),
    ECAP_BM_Attack16 UMETA(DisplayName = "BM Attack 16"),
    ECAP_BM_Attack17 UMETA(DisplayName = "BM Attack 17"),
    ECAP_BM_Attack18 UMETA(DisplayName = "BM Attack 18"),

    // WZ Attack Patterns
	ECAP_WZ_MeleeCombo1          UMETA(DisplayName = "WZ Melee Combo 1"),       // WZ 10->11->12
	ECAP_WZ_MeleeCombo2          UMETA(DisplayName = "WZ Melee Combo 2"),       // WZ 17->18

	ECAP_WZ_RangeCombo1          UMETA(DisplayName = "WZ Range Combo 1"),       // WZ 01->02
	ECAP_WZ_RangeCombo2          UMETA(DisplayName = "WZ Range Combo 2"),       // WZ 15->16
	
    ECAP_WZ_Attack01 UMETA(DisplayName = "WZ Attack 01"),
    ECAP_WZ_Attack02 UMETA(DisplayName = "WZ Attack 02"),
    ECAP_WZ_Attack03 UMETA(DisplayName = "WZ Attack 03"),
    ECAP_WZ_Attack04 UMETA(DisplayName = "WZ Attack 04"),
    ECAP_WZ_Attack05 UMETA(DisplayName = "WZ Attack 05"),
    ECAP_WZ_Attack06 UMETA(DisplayName = "WZ Attack 06"),
    ECAP_WZ_Attack07 UMETA(DisplayName = "WZ Attack 07"),
    ECAP_WZ_Attack08 UMETA(DisplayName = "WZ Attack 08"),
    ECAP_WZ_Attack09 UMETA(DisplayName = "WZ Attack 09"),
    ECAP_WZ_Attack10 UMETA(DisplayName = "WZ Attack 10"),
    ECAP_WZ_Attack11 UMETA(DisplayName = "WZ Attack 11"),
    ECAP_WZ_Attack12 UMETA(DisplayName = "WZ Attack 12"),
    ECAP_WZ_Attack13 UMETA(DisplayName = "WZ Attack 13"),
    ECAP_WZ_Attack14 UMETA(DisplayName = "WZ Attack 14"),
    ECAP_WZ_Attack15 UMETA(DisplayName = "WZ Attack 15"),
    ECAP_WZ_Attack16 UMETA(DisplayName = "WZ Attack 16"),
    ECAP_WZ_Attack17 UMETA(DisplayName = "WZ Attack 17"),
    ECAP_WZ_Attack18 UMETA(DisplayName = "WZ Attack 18"),
	ECAP_WZ_Attack19 UMETA(DisplayName = "WZ Attack 19"),
	ECAP_WZ_Attack20 UMETA(DisplayName = "WZ Attack 20"),
	ECAP_WZ_Attack21 UMETA(DisplayName = "WZ Attack 21"),
	ECAP_WZ_Attack22 UMETA(DisplayName = "WZ Attack 22"),
	ECAP_WZ_Attack23 UMETA(DisplayName = "WZ Attack 23"),
	
};

UCLASS()
class STILLLOADING_API ASLCompanionCharacter : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLCompanionCharacter();

	// 전투 모드 전환
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCombatMode(bool bInCombat);
	// 현재 태그 확인
	UFUNCTION(BlueprintCallable, Category = "Combat|GameplayTags")
	bool HasGameplayTag(const FGameplayTag& TagToCheck) const;

	// 기절 스킬 발동
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CastStunSpell(AActor* Target);

	// 추적 발사체 발사
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireProjectile(EAttackAnimType AttackAnimType);

	UFUNCTION(BlueprintCallable, Category = "State")
	bool GetIsBattleMage();

	UFUNCTION(BlueprintCallable, Category = "State")
	void SetIsBattleMage(bool bInBattleMage);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	ECompanionActionPattern SelectRandomPattern(const TArray<ECompanionActionPattern>& Patterns);

	UFUNCTION(BlueprintCallable, Category = "Combat")
    ECompanionActionPattern SelectPatternByDistance(
        float DistanceToTarget,
        const TArray<ECompanionActionPattern>& CloseRangePatterns,
        const TArray<ECompanionActionPattern>& MidRangePatterns, 
        const TArray<ECompanionActionPattern>& LongRangePatterns,
        float MidRangeThreshold = 300.0f,
        float LongRangeThreshold = 600.0f
    );
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType) override;
	
	// 자동 기절 타이머
	void AutoStunNearbyEnemy();

	// 추적 발사체 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ASLAIProjectile> ProjectileClass;

	// 발사 소켓 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName ProjectileSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsBattleMage;
	
	// 자동 기절 주기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AutoStunInterval;

	// 기절 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float StunDuration;

	// 기절 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> StunEffect;

	// 게임플레이 태그 컨테이너
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|GameplayTags")
	FGameplayTagContainer CurrentGameplayTags;
	
private:
	UPROPERTY()
	FTimerHandle AutoStunTimer;
	UPROPERTY()
	bool bIsInCombat;

	UPROPERTY()
	ECompanionActionPattern LastMidRangePattern;
	UPROPERTY()
	ECompanionActionPattern LastLongRangePattern;
};