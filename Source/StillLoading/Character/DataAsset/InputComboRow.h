#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "InputComboRow.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	ST_None UMETA(DisplayName = "None"),
	ST_Attack UMETA(DisplayName = "Attack"),
	ST_Airborne UMETA(DisplayName = "Airborne"),
	ST_AirUp UMETA(DisplayName = "AirUp"),
	ST_Airdown UMETA(DisplayName = "Airdown"),
	ST_Dodge UMETA(DisplayName = "Dodge"),
	ST_Block UMETA(DisplayName = "Block"),
	ST_PointMove UMETA(DisplayName = "PointMove"),
	ST_BlastSword UMETA(DisplayName = "BlastSword"),
	ST_BlastShield UMETA(DisplayName = "BlastShield"),
};

USTRUCT(BlueprintType)
struct FBufferedInput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESkillType Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Timestamp;
};

USTRUCT(BlueprintType)
struct FInputComboRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ESkillType> InputSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillType ResultSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxTotalDuration = 0.5f;

	UPROPERTY(EditAnywhere)
	FGameplayTag RequiredPrimaryTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag RequiredSecondaryTag;
};
