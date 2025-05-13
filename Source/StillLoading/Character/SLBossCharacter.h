// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLAIBaseCharacter.h"
#include "SLBossCharacter.generated.h"

// 보스의 고유 공격 패턴
UENUM(BlueprintType)
enum class EBossAttackPattern : uint8
{
	None                UMETA(DisplayName = "None"), // 공격 없음 또는 기본 상태
	Attack_01           UMETA(DisplayName = "Attack 01"),
	Attack_02           UMETA(DisplayName = "Attack 02"),
	Attack_03           UMETA(DisplayName = "Attack 03"),
	Attack_04           UMETA(DisplayName = "Attack 04"),
	DashAttack          UMETA(DisplayName = "Dash Attack"),
	FootAttack_Left     UMETA(DisplayName = "Foot Attack - Left"),
	FootAttack_Right    UMETA(DisplayName = "Foot Attack - Right"),
	GroundSlam_01       UMETA(DisplayName = "Ground Slam 01"),
	GroundSlam_02       UMETA(DisplayName = "Ground Slam 02"),
	JumpAttack          UMETA(DisplayName = "Jump Attack"),
	ThrowStone          UMETA(DisplayName = "Throw Stone"),
	Whirlwind           UMETA(DisplayName = "Whirlwind Attack")
};

UCLASS()
class STILLLOADING_API ASLBossCharacter : public ASLAIBaseCharacter
{
	GENERATED_BODY()

public:
	ASLBossCharacter();
	void SetBossAttackPattern(EBossAttackPattern NewPattern);

private:
	// --- Combat Data ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EBossAttackPattern BossAttackPattern;
};
