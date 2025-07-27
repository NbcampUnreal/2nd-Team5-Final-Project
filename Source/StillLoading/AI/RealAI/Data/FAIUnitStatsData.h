#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FAIUnitStatsData.generated.h"

USTRUCT(BlueprintType)
struct FAIUnitStatsData : public FTableRowBase
{
	GENERATED_BODY()

	// 체력 (최소/최대)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float HealthMin = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float HealthMax = 150.0f;

	// 이동 속도 (최소/최대)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MovementSpeedMin = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MovementSpeedMax = 400.0f;
};
