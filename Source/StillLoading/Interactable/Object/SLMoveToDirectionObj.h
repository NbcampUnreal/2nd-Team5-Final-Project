// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SLMoveToDirectionObj.generated.h"

UCLASS()
class STILLLOADING_API ASLMoveToDirectionObj : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
public:
	ASLMoveToDirectionObj();

protected:
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Reactive")
	float MoveSpeed = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Reactive")
	float MoveDistance = 100.0f;

	FVector StartLocation = FVector::ZeroVector;
	FVector MoveDirection = FVector::ZeroVector;

	bool bIsActivated = false;
};
