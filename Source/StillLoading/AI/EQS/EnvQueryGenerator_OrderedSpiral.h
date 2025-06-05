// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "EnvQueryGenerator_OrderedSpiral.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API UEnvQueryGenerator_OrderedSpiral : public UEnvQueryGenerator
{
	GENERATED_BODY()

public:
	UEnvQueryGenerator_OrderedSpiral();

	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Spiral")
	float NumberOfTurns = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spiral")
	int32 PointCount = 30;

	UPROPERTY(EditDefaultsOnly, Category = "Spiral")
	float MinRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spiral")
	float MaxRadius = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Generator")
	TSubclassOf<UEnvQueryContext> CenterContext;
};
