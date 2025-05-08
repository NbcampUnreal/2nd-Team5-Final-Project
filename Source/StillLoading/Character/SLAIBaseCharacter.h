// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLBaseCharacter.h"
#include "SLAIBaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Moving      UMETA(DisplayName = "Moving"),
	Combat		UMETA(DisplayName = "Combat"),
	Dead        UMETA(DisplayName = "Dead")
};

UCLASS()
class STILLLOADING_API ASLAIBaseCharacter : public ASLBaseCharacter
{
	GENERATED_BODY()

public:
	ASLAIBaseCharacter();

	// 플레이어를 바라보는 상태인지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool LookAtPlayer;

	//공격중인지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool IsAttacking;
};
