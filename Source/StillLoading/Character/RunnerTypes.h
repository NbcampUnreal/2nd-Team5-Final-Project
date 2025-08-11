// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RunnerTypes.generated.h"

UENUM(BlueprintType)
enum class EHurdleState : uint8
{
	None,
	Jump,
	Attack,
	Sliding
};

UENUM(BlueprintType)
enum class ERunnerAction : uint8
{
	None,
	Jump,
	Attack,
	Slide
};