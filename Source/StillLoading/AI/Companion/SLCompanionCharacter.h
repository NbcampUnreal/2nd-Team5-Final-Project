// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Character/SLAIBaseCharacter.h"
#include "SLCompanionCharacter.generated.h"

// Companion의 행동 패턴
UENUM(BlueprintType)
enum class ECompanionActionPattern : uint8
{
	ECAP_None                UMETA(DisplayName = "None"),                // 행동 없음 또는 기본 상태
	ECAP_MagicAttack_01      UMETA(DisplayName = "Magic Attack 01"),     // 마법 공격 1
	ECAP_MagicAttack_02      UMETA(DisplayName = "Magic Attack 02"),     // 마법 공격 2
	ECAP_MagicAttack_03      UMETA(DisplayName = "Magic Attack 03"),     // 마법 공격 3
	ECAP_CastStun            UMETA(DisplayName = "Cast Stun Spell"),     // 기절 마법 시전
	ECAP_HighRightCast       UMETA(DisplayName = "High Right Cast"),     // 오른쪽 상단 캐스팅
	ECAP_OverHeadCast        UMETA(DisplayName = "Overhead Cast"),       // 머리 위 캐스팅
	ECAP_PointCast           UMETA(DisplayName = "Point Cast"),          // 지시 캐스팅
	ECAP_WaveCast            UMETA(DisplayName = "Wave Cast"),           // 파동 캐스팅
	ECAP_HomingProjectile    UMETA(DisplayName = "Homing Projectile"),   // 추적 발사체
	ECAP_DefensiveBarrier    UMETA(DisplayName = "Defensive Barrier"),   // 방어 장벽
	ECAP_AreaStun            UMETA(DisplayName = "Area Stun")            // 광역 기절
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

	// 기절 스킬 발동
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CastStunSpell(AActor* Target);

	// 추적 발사체 발사
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireProjectile(EAttackAnimType AttackAnimType);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	ECompanionActionPattern SelectRandomPattern(const TArray<ECompanionActionPattern>& Patterns);
protected:
	virtual void BeginPlay() override;

	virtual void CharacterHit(AActor* DamageCauser, float DamageAmount, const FHitResult& HitResult, EAttackAnimType AnimType) override;

	// 자동 기절 타이머
	void AutoStunNearbyEnemy();

	// 추적 발사체 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ASLAIProjectile> ProjectileClass;

	// 발사 소켓 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName ProjectileSocketName;

	// 자동 기절 주기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AutoStunInterval;

	// 기절 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float StunDuration;

	// 기절 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> StunEffect;

private:
	FTimerHandle AutoStunTimer;
	bool bIsInCombat;
};
