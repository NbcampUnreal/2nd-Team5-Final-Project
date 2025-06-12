// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLReactiveObjectRock.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLReactiveObjectRock : public ASLBaseReactiveObject
{
	GENERATED_BODY()
	
	ASLReactiveObjectRock();

	UFUNCTION(BlueprintCallable)
	void PushInDirection(FVector Direction);

	UFUNCTION()
	void MoveRock(float DeltaTime);

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType) override;



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
