// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SLGameSubjectBase.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLGameSubjectBase : public UObject
{
	GENERATED_BODY()

public:
	USLGameSubjectBase();
	FORCEINLINE FString GetDescription() const { return Description; }
	
protected:
	virtual void StartSubject();
	virtual void EndSubject();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Subject")
	FString Description;
};
