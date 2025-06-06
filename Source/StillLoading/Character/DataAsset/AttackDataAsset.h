#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttackDataAsset.generated.h"

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
    AAT_Attack_Plunge           UMETA(DisplayName = "Ai Attack Plunge"),
    AAT_Attack_Air              UMETA(DisplayName = "Ai Attack Air"),

    // BM Attack Patterns
    AAT_BM_MeleeCombo1          UMETA(DisplayName = "BM Melee Combo 1"),// BM 01->02->03->04->10
    AAT_BM_MeleeCombo2          UMETA(DisplayName = "BM Melee Combo 2"),// BM 13->05->06->08->12
    AAT_BM_MeleeCombo3          UMETA(DisplayName = "BM Melee Combo 3"),// BM 05->06->07->08->09
    AAT_BM_QuickCombo1          UMETA(DisplayName = "BM Quick Combo 1"),// BM 10->11->12
    AAT_BM_HeavyCombo1          UMETA(DisplayName = "BM Heavy Combo 1"),// BM 13->14->15->16
    AAT_BM_FinisherCombo1       UMETA(DisplayName = "BM Finisher Combo 1"),// BM 17->18

    AAT_BM_Attack01             UMETA(DisplayName = "BM Attack 01"),
    AAT_BM_Attack02             UMETA(DisplayName = "BM Attack 02"),
    AAT_BM_Attack03             UMETA(DisplayName = "BM Attack 03"),
    AAT_BM_Attack04             UMETA(DisplayName = "BM Attack 04"),
    AAT_BM_Attack05             UMETA(DisplayName = "BM Attack 05"),
    AAT_BM_Attack06             UMETA(DisplayName = "BM Attack 06"),
    AAT_BM_Attack07             UMETA(DisplayName = "BM Attack 07"),
    AAT_BM_Attack08             UMETA(DisplayName = "BM Attack 08"),
    AAT_BM_Attack09             UMETA(DisplayName = "BM Attack 09"),
    AAT_BM_Attack10             UMETA(DisplayName = "BM Attack 10"),
    AAT_BM_Attack11             UMETA(DisplayName = "BM Attack 11"),
    AAT_BM_Attack12             UMETA(DisplayName = "BM Attack 12"),
    AAT_BM_Attack13             UMETA(DisplayName = "BM Attack 13"),
    AAT_BM_Attack14             UMETA(DisplayName = "BM Attack 14"),
    AAT_BM_Attack15             UMETA(DisplayName = "BM Attack 15"),
    AAT_BM_Attack16             UMETA(DisplayName = "BM Attack 16"),
    AAT_BM_Attack17             UMETA(DisplayName = "BM Attack 17"),
    AAT_BM_Attack18             UMETA(DisplayName = "BM Attack 18"),

    // WZ Attack Patterns
    AAT_WZ_MeleeCombo1          UMETA(DisplayName = "WZ Melee Combo 1"),
    AAT_WZ_MeleeCombo2          UMETA(DisplayName = "WZ Melee Combo 2"),
    AAT_WZ_RangeCombo1          UMETA(DisplayName = "WZ Range Combo 1"),
    AAT_WZ_RangeCombo2          UMETA(DisplayName = "WZ Range Combo 2"),

    AAT_WZ_Attack01             UMETA(DisplayName = "WZ Attack 01"),
    AAT_WZ_Attack02             UMETA(DisplayName = "WZ Attack 02"),
    AAT_WZ_Attack03             UMETA(DisplayName = "WZ Attack 03"),
    AAT_WZ_Attack04             UMETA(DisplayName = "WZ Attack 04"),
    AAT_WZ_Attack05             UMETA(DisplayName = "WZ Attack 05"),
    AAT_WZ_Attack06             UMETA(DisplayName = "WZ Attack 06"),
    AAT_WZ_Attack07             UMETA(DisplayName = "WZ Attack 07"),
    AAT_WZ_Attack08             UMETA(DisplayName = "WZ Attack 08"),
    AAT_WZ_Attack09             UMETA(DisplayName = "WZ Attack 09"),
    AAT_WZ_Attack10             UMETA(DisplayName = "WZ Attack 10"),
    AAT_WZ_Attack11             UMETA(DisplayName = "WZ Attack 11"),
    AAT_WZ_Attack12             UMETA(DisplayName = "WZ Attack 12"),
    AAT_WZ_Attack13             UMETA(DisplayName = "WZ Attack 13"),
    AAT_WZ_Attack14             UMETA(DisplayName = "WZ Attack 14"),
    AAT_WZ_Attack15             UMETA(DisplayName = "WZ Attack 15"),
    AAT_WZ_Attack16             UMETA(DisplayName = "WZ Attack 16"),
    AAT_WZ_Attack17             UMETA(DisplayName = "WZ Attack 17"),
    AAT_WZ_Attack18             UMETA(DisplayName = "WZ Attack 18"),
    AAT_WZ_Attack19             UMETA(DisplayName = "WZ Attack 19"),
    AAT_WZ_Attack20             UMETA(DisplayName = "WZ Attack 20"),
    AAT_WZ_Attack21             UMETA(DisplayName = "WZ Attack 21"),
    AAT_WZ_Attack22             UMETA(DisplayName = "WZ Attack 22"),
    AAT_WZ_Attack23             UMETA(DisplayName = "WZ Attack 23"),

    AAT_WZ_Loop_Attack04        UMETA(DisplayName = "WZ Loop Attack 04"),

    AAT_WZ_Special_Patterns1    UMETA(DisplayName = "WZ Special Patterns1"),
    AAT_WZ_Special_Patterns2    UMETA(DisplayName = "WZ Special Patterns2"),
    AAT_WZ_Special_Patterns3    UMETA(DisplayName = "WZ Special Patterns3"),
    AAT_WZ_Special_Patterns4    UMETA(DisplayName = "WZ Special Patterns4"),
    AAT_WZ_Special_Patterns5    UMETA(DisplayName = "WZ Special Patterns5"),
    
    AAT_WZ_Attack_Aim01         UMETA(DisplayName = "WZ Attack Aim 01"),
    AAT_WZ_Attack_Aim02         UMETA(DisplayName = "WZ Attack Aim 02"),
    AAT_WZ_Attack_Aim03         UMETA(DisplayName = "WZ Attack Aim 03"),
    AAT_WZ_Attack_Aim04         UMETA(DisplayName = "WZ Attack Aim 04"),

	// 잡몹용
	AAT_AINormal UMETA(DisplayName = "AI Normal Attack"),
	AAT_AISpecial UMETA(DisplayName = "AI Special Attack"),
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
