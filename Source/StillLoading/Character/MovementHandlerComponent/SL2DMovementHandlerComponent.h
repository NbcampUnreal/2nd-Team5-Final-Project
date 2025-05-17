// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SL2DMovementHandlerComponent.generated.h"


class ASLPlayerCharacterBase;
struct FInputActionValue;
enum class EInputActionType : uint8;
class UInputAction;
class UDynamicIMCComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STILLLOADING_API USL2DMovementHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USL2DMovementHandlerComponent();

	UFUNCTION()
	void Attack();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	UFUNCTION()
	void OnActionTriggered(EInputActionType ActionType, FInputActionValue Value);
	UFUNCTION()
	void OnActionStarted(EInputActionType ActionType);
	UFUNCTION()
	void OnActionCompleted(EInputActionType ActionType);
	UFUNCTION()
	void BindIMCComponent();


private:

	UFUNCTION()
	void Look(const FVector2D& Value);

	UFUNCTION()
	void Move(const float AxisValue, const EInputActionType ActionType);
	
	UFUNCTION()
	void MoveGrid(FVector InputDir);

	UFUNCTION()
	void RotateToDirection(FVector Direction);

	UFUNCTION()
	void Interact();

	UFUNCTION()
	void ToggleMenu();

	UFUNCTION()
	void SetTopDownView();


	UPROPERTY()
	TObjectPtr<ASLPlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	bool bIsMoving = false;

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	float MoveDuration = 0.15f; // 이동에 걸리는 시간

	UPROPERTY()
	float MoveElapsed = 0.0f;

	UPROPERTY()
	float StepDistance = 100.0f;
	               
	UPROPERTY()
	FVector NextMove;

	UPROPERTY()
	float InputBufferDuration = 0.3f;

	int32 CurrentIndex = 0; // Test용


	
};
