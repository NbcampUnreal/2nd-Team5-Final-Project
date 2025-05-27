// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLAICharacterAnimInstance.h"
#include "AI/Companion/SLCompanionCharacter.h"
#include "SLCompanionAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLCompanionAnimInstance : public USLAICharacterAnimInstance
{
	GENERATED_BODY()

public:
	USLCompanionAnimInstance();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	ECompanionActionPattern GetCompanionPattern() const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCompanionPattern(const ECompanionActionPattern CompanionPattern);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	 ECompanionActionPattern CompanionPattern;
};
