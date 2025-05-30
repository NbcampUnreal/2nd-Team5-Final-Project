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
	AAT_Attack_01 UMETA(DisplayName = "Attack 01"), // 날라가는거
	AAT_Attack_02 UMETA(DisplayName = "Attack 02"), // 날라가는거
	AAT_Attack_03 UMETA(DisplayName = "Attack 03"), // 기절
	AAT_Attack_04 UMETA(DisplayName = "Attack 04"), // 날라가는거
	AAT_DashAttack UMETA(DisplayName = "Dash Attack"), // 날라가는거
	AAT_FootAttack_Left UMETA(DisplayName = "Foot Attack - Left"), // 중간거
	AAT_FootAttack_Right UMETA(DisplayName = "Foot Attack - Right"), // 중간거
	AAT_GroundSlam_01 UMETA(DisplayName = "Ground Slam 01"), // 중간거
	AAT_GroundSlam_02 UMETA(DisplayName = "Ground Slam 02"), // 중간거
	AAT_JumpAttack UMETA(DisplayName = "Jump Attack"), // 중간거
	AAT_ThrowStone UMETA(DisplayName = "Throw Stone"), // 날라가는거
	AAT_Whirlwind UMETA(DisplayName = "Whirlwind Attack"), // 중간거

	AAT_AINormal UMETA(DisplayName = "AI Normal Attack"),
	AAT_AISpecial UMETA(DisplayName = "AI Special Attack"),

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
