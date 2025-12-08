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
	void BeginOverlapPlayer();

	UFUNCTION(BlueprintCallable)
	void EndOverlapPlayer();

	UFUNCTION()
	void Move(float DeltaTime);

	UFUNCTION()
	void Deceleration(float DeltaTime);

	UFUNCTION()
	void Accelerate(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Spline")
	TObjectPtr<AActor> TargetSplineActor;

	UPROPERTY(EditAnywhere, Category = "Spline")
	TObjectPtr<USplineComponent> TargetSpline;

	UPROPERTY()
	FTimerHandle SpeedTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DefaultSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float IncreasePerSecond = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float InterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DecreasePerSecond = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DeInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TargetSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool bDeceleration = false;


private:

	UPROPERTY(VisibleAnywhere, Category = "AI")
	bool bShouldMove = false;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	float CurrentDistance = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	int32 OverlapEnemyCount = 0;

	bool bIsPlayerOn = false;
	bool bIsEnemyOn = false;

};
