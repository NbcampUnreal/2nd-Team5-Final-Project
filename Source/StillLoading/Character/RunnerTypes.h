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
enum class EQTERating : uint8
{
	None,
	Perfect,
	Good,
	Early,
	WrongKey,
	Miss
};


UENUM(BlueprintType)
enum class ERunnerMontageSection : uint8
{
	None,

	Jump_Low,
	Jump_Mid,
	Jump_High,

	Slide_Short,
	Slide_Mid,
	Slide_Long,

	Attack_Center,
	Attack_Right,
	Attack_Left
};

FORCEINLINE FName GetMontageSectionName(ERunnerMontageSection Section)
{
	switch (Section)
	{
		case ERunnerMontageSection::Jump_Low: return TEXT("Jump_Low");
		case ERunnerMontageSection::Jump_Mid: return TEXT("Jump_Mid");
		case ERunnerMontageSection::Jump_High: return TEXT("Jump_High");
		case ERunnerMontageSection::Slide_Short: return TEXT("Slide_Short");
		case ERunnerMontageSection::Slide_Mid: return TEXT("Slide_Mid");
		case ERunnerMontageSection::Slide_Long: return TEXT("Slide_Long");
		case ERunnerMontageSection::Attack_Center: return TEXT("Attack_Center");
		case ERunnerMontageSection::Attack_Left: return TEXT("Attack_Left");
		case ERunnerMontageSection::Attack_Right: return TEXT("Attack_Right");
		default: return TEXT("None");
	}
}