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
	AAT_Attack_01 UMETA(DisplayName = "Attack 01"),
	AAT_Attack_02 UMETA(DisplayName = "Attack 02"),
	AAT_Attack_03 UMETA(DisplayName = "Attack 03"),
	AAT_Attack_04 UMETA(DisplayName = "Attack 04"),
	AAT_DashAttack UMETA(DisplayName = "Dash Attack"),
	AAT_FootAttack_Left UMETA(DisplayName = "Foot Attack - Left"),
	AAT_FootAttack_Right UMETA(DisplayName = "Foot Attack - Right"),
	AAT_GroundSlam_01 UMETA(DisplayName = "Ground Slam 01"),
	AAT_GroundSlam_02 UMETA(DisplayName = "Ground Slam 02"),
	AAT_JumpAttack UMETA(DisplayName = "Jump Attack"),
	AAT_ThrowStone UMETA(DisplayName = "Throw Stone"),
	AAT_Whirlwind UMETA(DisplayName = "Whirlwind Attack"),

	// Object
	AAT_Activate UMETA(DisplayName = "Activate"),
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
