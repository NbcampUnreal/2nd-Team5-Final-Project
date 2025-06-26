#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttackDataAsset.generated.h"

UENUM(BlueprintType)
enum class EHitAnimType : uint8 // 애니메이션 기준
{
	HAT_None,

	// Character
	HAT_WeakHit UMETA(DisplayName = "Weak Hit"),
	HAT_AirHit UMETA(DisplayName = "Weak Hit"),
	HAT_HardHit UMETA(DisplayName = "Hard Hit"),
	HAT_Exhausterd UMETA(DisplayName = "Exhausterd Hit"), // 탈진
	HAT_FallBack UMETA(DisplayName = "FallBack Hit"),
	HAT_AirBorne UMETA(DisplayName = "Air Borne Hit"),
	HAT_AirUp UMETA(DisplayName = "Air Up Hit"),

	// AI
	HAT_KillMotionA UMETA(DisplayName = "Kill MotionA"),
	HAT_KillMotionB UMETA(DisplayName = "Kill MotionB"),
	HAT_KillMotionC UMETA(DisplayName = "Kill MotionC"),
	HAT_Parry UMETA(DisplayName = "Parry Hit"),
};

UENUM(BlueprintType)
enum class EAttackAnimType : uint8 // 애니메이션 기준
{
	// Character
	AAT_NormalAttack1 UMETA(DisplayName = "Normal Attack1"),
	AAT_NormalAttack2 UMETA(DisplayName = "Normal Attack2"),
	AAT_NormalAttack3 UMETA(DisplayName = "Normal Attack3"),
	AAT_SpecialAttack1 UMETA(DisplayName = "Special Attack1"),
	AAT_SpecialAttack2 UMETA(DisplayName = "Special Attack2"),
	AAT_SpecialAttack3 UMETA(DisplayName = "Special Attack3"),
	AAT_AirAttack1 UMETA(DisplayName = "Air Attack1"),
	AAT_AirAttack2 UMETA(DisplayName = "Air Attack2"),
	AAT_AirAttack3 UMETA(DisplayName = "Air Attack3"),
	AAT_Airborn UMETA(DisplayName = "Airborn"),
	AAT_Skill1 UMETA(DisplayName = "Skill1"),
	AAT_Skill2 UMETA(DisplayName = "Skill2"),

	// AI
	// 모션: 날라가는거, 약한거, 중간거 (한발짝), 기절 
	AAT_Attack_01 UMETA(DisplayName = "AI Boss Attack 01"), // 날라가는거
	AAT_Attack_02 UMETA(DisplayName = "AI Boss Attack 02"), // 날라가는거
	AAT_Attack_03 UMETA(DisplayName = "AI Boss Attack 03"), // 기절
	AAT_Attack_04 UMETA(DisplayName = "AI Boss Attack 04"), // 날라가는거
	AAT_DashAttack UMETA(DisplayName = "AI Boss Dash Attack"), // 날라가는거
	AAT_FootAttack_Left UMETA(DisplayName = "AI Boss Foot Attack - Left"), // 중간거
	AAT_FootAttack_Right UMETA(DisplayName = "AI Boss Foot Attack - Right"), // 중간거
	AAT_GroundSlam_02 UMETA(DisplayName = "AI Boss Ground Slam 02"), // 중간거
	AAT_GroundSlam_01 UMETA(DisplayName = "AI Boss Ground Slam 01"), // 중간거
	AAT_JumpAttack UMETA(DisplayName = "Ai Boss Jump Attack"), // 중간거
	AAT_ThrowStone UMETA(DisplayName = "Ai Boss Throw Stone"), // 날라가는거
	AAT_Whirlwind UMETA(DisplayName = "Ai Boss Whirlwind Attack"), // 중간거

	AAT_AIProjectile UMETA(DisplayName = "AI Projectile"),
	// 동료 AI
	AAT_Attack_Plunge UMETA(DisplayName = "Ai Attack Plunge"),
	AAT_Attack_Air UMETA(DisplayName = "Ai Attack Air"),

	// BM Attack Patterns
	AAT_BM_MeleeCombo1 UMETA(DisplayName = "BM Melee Combo 1"), // BM 01->02->03->04->10
	AAT_BM_MeleeCombo2 UMETA(DisplayName = "BM Melee Combo 2"), // BM 13->05->06->08->12
	AAT_BM_MeleeCombo3 UMETA(DisplayName = "BM Melee Combo 3"), // BM 05->06->07->08->09
	AAT_BM_QuickCombo1 UMETA(DisplayName = "BM Quick Combo 1"), // BM 10->11->12
	AAT_BM_HeavyCombo1 UMETA(DisplayName = "BM Heavy Combo 1"), // BM 13->14->15->16
	AAT_BM_FinisherCombo1 UMETA(DisplayName = "BM Finisher Combo 1"), // BM 17->18

	AAT_BM_Attack01 UMETA(DisplayName = "BM Attack 01"), // 약한거
	AAT_BM_Attack02 UMETA(DisplayName = "BM Attack 02"), // 약한거
	AAT_BM_Attack03 UMETA(DisplayName = "BM Attack 03"), // 약한거
	AAT_BM_Attack04 UMETA(DisplayName = "BM Attack 04"), // 중간거 (한발짝)
	AAT_BM_Attack05 UMETA(DisplayName = "BM Attack 05"), // 약한거
	AAT_BM_Attack06 UMETA(DisplayName = "BM Attack 06"), // 약한거
	AAT_BM_Attack07 UMETA(DisplayName = "BM Attack 07"), // 중간거 (한발짝)
	AAT_BM_Attack08 UMETA(DisplayName = "BM Attack 08"), // 중간거 (한발짝)
	AAT_BM_Attack09 UMETA(DisplayName = "BM Attack 09"), // 약한거
	AAT_BM_Attack10 UMETA(DisplayName = "BM Attack 10"), // 중간거 (한발짝)
	AAT_BM_Attack11 UMETA(DisplayName = "BM Attack 11"), // 약한거
	AAT_BM_Attack12 UMETA(DisplayName = "BM Attack 12"), // 약한거
	AAT_BM_Attack13 UMETA(DisplayName = "BM Attack 13"), // 중간거 (한발짝)
	AAT_BM_Attack14 UMETA(DisplayName = "BM Attack 14"), // 중간거 (한발짝)
	AAT_BM_Attack15 UMETA(DisplayName = "BM Attack 15"), // 중간거 (한발짝)
	AAT_BM_Attack16 UMETA(DisplayName = "BM Attack 16"), // 중간거 (한발짝)
	AAT_BM_Attack17 UMETA(DisplayName = "BM Attack 17"), // 중간거 (한발짝)
	AAT_BM_Attack18 UMETA(DisplayName = "BM Attack 18"), // 중간거 (한발짝)

	// WZ Attack Patterns
	AAT_WZ_MeleeCombo1 UMETA(DisplayName = "WZ Melee Combo 1"),
	AAT_WZ_MeleeCombo2 UMETA(DisplayName = "WZ Melee Combo 2"),
	AAT_WZ_RangeCombo1 UMETA(DisplayName = "WZ Range Combo 1"),
	AAT_WZ_RangeCombo2 UMETA(DisplayName = "WZ Range Combo 2"),

	AAT_WZ_Attack01 UMETA(DisplayName = "WZ Attack 01"), // 중간거 (한발짝)
	AAT_WZ_Attack02 UMETA(DisplayName = "WZ Attack 02"), // 중간거 (한발짝)
	AAT_WZ_Attack03 UMETA(DisplayName = "WZ Attack 03"), // 중간거 (한발짝)
	AAT_WZ_Attack04 UMETA(DisplayName = "WZ Attack 04"), // 중간거 (한발짝)
	AAT_WZ_Attack05 UMETA(DisplayName = "WZ Attack 05"), // 날라가는거
	AAT_WZ_Attack06 UMETA(DisplayName = "WZ Attack 06"), // 중간거 (한발짝)
	AAT_WZ_Attack07 UMETA(DisplayName = "WZ Attack 07"), // 중간거 (한발짝)
	AAT_WZ_Attack08 UMETA(DisplayName = "WZ Attack 08"), //기절
	AAT_WZ_Attack09 UMETA(DisplayName = "WZ Attack 09"), // 날라가는거
	AAT_WZ_Attack10 UMETA(DisplayName = "WZ Attack 10"), // 약한거
	AAT_WZ_Attack11 UMETA(DisplayName = "WZ Attack 11"), // 약한거
	AAT_WZ_Attack12 UMETA(DisplayName = "WZ Attack 12"), // 중간거 (한발짝)
	AAT_WZ_Attack13 UMETA(DisplayName = "WZ Attack 13"), // 중간거 (한발짝)
	AAT_WZ_Attack14 UMETA(DisplayName = "WZ Attack 14"), // 중간거 (한발짝)
	AAT_WZ_Attack15 UMETA(DisplayName = "WZ Attack 15"), // 중간거 (한발짝)
	AAT_WZ_Attack16 UMETA(DisplayName = "WZ Attack 16"), // 중간거 (한발짝)
	AAT_WZ_Attack17 UMETA(DisplayName = "WZ Attack 17"), // 약한거
	AAT_WZ_Attack18 UMETA(DisplayName = "WZ Attack 18"), // 약한거
	AAT_WZ_Attack19 UMETA(DisplayName = "WZ Attack 19"), // 약한거
	AAT_WZ_Attack20 UMETA(DisplayName = "WZ Attack 20"), // 중간거 (한발짝)
	AAT_WZ_Attack21 UMETA(DisplayName = "WZ Attack 21"), // 중간거 (한발짝)
	AAT_WZ_Attack22 UMETA(DisplayName = "WZ Attack 22"), // 중간거 (한발짝)
	AAT_WZ_Attack23 UMETA(DisplayName = "WZ Attack 23"), // 중간거 (한발짝)

	AAT_WZ_Loop_Attack04 UMETA(DisplayName = "WZ Loop Attack 04"), // 기절

	AAT_WZ_Special_Patterns1 UMETA(DisplayName = "WZ Special Patterns1"), // 중간거 (한발짝)
	AAT_WZ_Special_Patterns2 UMETA(DisplayName = "WZ Special Patterns2"), // 중간거 (한발짝)
	AAT_WZ_Special_Patterns3 UMETA(DisplayName = "WZ Special Patterns3"), // 중간거 (한발짝)
	AAT_WZ_Special_Patterns4 UMETA(DisplayName = "WZ Special Patterns4"), // 중간거 (한발짝)
	AAT_WZ_Special_Patterns5 UMETA(DisplayName = "WZ Special Patterns5"), // 중간거 (한발짝)

	AAT_WZ_Attack_Aim01 UMETA(DisplayName = "WZ Attack Aim 01"),
	AAT_WZ_Attack_Aim02 UMETA(DisplayName = "WZ Attack Aim 02"),
	AAT_WZ_Attack_Aim03 UMETA(DisplayName = "WZ Attack Aim 03"),
	AAT_WZ_Attack_Aim04 UMETA(DisplayName = "WZ Attack Aim 04"),

	//도플갱어 AI 공격
	AAT_DG_Combo_Attack_01 UMETA(DisplayName = "DG Combo Attack 01"), // 약한거
	AAT_DG_Combo_Attack_02 UMETA(DisplayName = "DG Combo Attack 02"), // 약한거
	AAT_DG_Combo_Attack_03 UMETA(DisplayName = "DG Combo Attack 03"), // 약한거
	AAT_DG_Combo_Attack_04 UMETA(DisplayName = "DG Combo Attack 04"), // 약한거
	AAT_DG_Combo_Attack_Air UMETA(DisplayName = "DG Combo Attack Air"), // 약한거

	// 특수 공격
	AAT_DG_Attack_Up_01 UMETA(DisplayName = "DG Attack Up 01"), // 날라가는거 위로
	AAT_DG_Attack_Up_Floor_To_Air_02 UMETA(DisplayName = "DG Attack Up Floor To Air 02"), // 날라가는거 위로
	AAT_DG_Attack_Air_To_Floor UMETA(DisplayName = "DG Attack Air To Floor"), // 약한거
	AAT_DG_Run_Attack_02 UMETA(DisplayName = "DG Run Attack 02"), // 중간거 (한발짝)
	AAT_DG_Dash_Air_Attack UMETA(DisplayName = "DG Dash Air Attack"), // 중간거 (한발짝)

	// 패리공격
	AAT_DG_Parry UMETA(DisplayName = "DG Parry"), // 기절

	// 잡몹용
	AAT_AINormal UMETA(DisplayName = "AI Normal Attack"),
	AAT_AISpecial UMETA(DisplayName = "AI Special Attack"),
	AAT_AIUlti UMETA(DisplayName = "AI Ulti"),
	AAT_Dead UMETA(DisplayName = "Dead"),

	// Object
	AAT_Activate UMETA(DisplayName = "Activate"),

	// 막타모션
	AAT_FinalAttackA UMETA(DisplayName = "FinalAttackA"), // 막타모션
	AAT_FinalAttackB UMETA(DisplayName = "FinalAttackB"), // 막타모션
	AAT_FinalAttackC UMETA(DisplayName = "FinalAttackC"), // 막타모션
	AAT_ParryAttack UMETA(DisplayName = "ParryAttack"), // 패링
};

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	EAttackAnimType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float DamageAmount;
};

UCLASS()
class STILLLOADING_API UAttackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<FAttackData> AttackDataList;
};
