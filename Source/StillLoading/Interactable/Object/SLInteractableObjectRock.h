// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SLInteractableObjectRock.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLInteractableObjectRock : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
	ASLInteractableObjectRock();

	UFUNCTION(BlueprintCallable)
	void PushInDirection(FVector Direction);

	UFUNCTION()
	void MoveRock(float DeltaTime);

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType) override;



private:
	bool bIsMoving = false;
	FVector MoveDirection;
	FVector StartLocation;
	FVector TargetLocation;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1.0f;

	UPROPERTY(EditAnywhere)
	float RockRadius = 250.0f;

	UPROPERTY(EditAnywhere)
	float PushDistance = 500.0f;
};
