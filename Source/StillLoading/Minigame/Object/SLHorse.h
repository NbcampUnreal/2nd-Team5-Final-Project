// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "SLHorse.generated.h"

class USplineComponent;
class UBoxComponent;

UCLASS()
class STILLLOADING_API ASLHorse : public ACharacter
{
	GENERATED_BODY()

public:
	ASLHorse();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnEnemyDetected();

	UFUNCTION(BlueprintCallable)
	void OnEnemyCleared();

	UFUNCTION(BlueprintCallable)
	void OnTrigger();

	UFUNCTION()
	void Move();

	UPROPERTY(EditAnywhere, Category = "Spline")
	TObjectPtr<AActor> TargetSplineActor;

	UPROPERTY(EditAnywhere, Category = "Spline")
	TObjectPtr<USplineComponent> TargetSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AutoSpeed = 100.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	bool bShouldMove = true;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	float CurrentDistance = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	int32 OverlapPawnCount = 0;

};
