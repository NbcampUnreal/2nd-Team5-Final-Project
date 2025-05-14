// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SL2DMovementHandlerComponent.generated.h"


class ASLBaseCharacter;
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
	void EnableRetroMovement();

	UFUNCTION()
	void DisableRetroMovement();

	UFUNCTION()
	void Look(const FVector2D& Value);

	UFUNCTION()
	void Move(const float AxisValue, const EInputActionType ActionType);
	
	UFUNCTION()
	void MoveInDirection(FVector Direction);

	UFUNCTION()
	void RotateToDirection(FVector Direction);

	UFUNCTION()
	bool CanMoveToLocation(FVector TargetLocation);

	UFUNCTION()
	void Interact();

	UFUNCTION()
	void ToggleMenu();

	UFUNCTION()
	void SetTopDownView();


	UPROPERTY()
	TObjectPtr<ASLBaseCharacter> OwnerCharacter;

	UPROPERTY()
	bool bIsMoving;

	UPROPERTY()
	float InputBufferDuration = 0.3f;

	int32 CurrentIndex = 0; // Test용

	UPROPERTY()
	float OriginalMaxWalkSpeed = 0.0f;

	UPROPERTY()
	float OriginalBrakingFrictionFactor = 0.0f;

	UPROPERTY()
	float OriginalGravityScale = 0.0f;

	UPROPERTY()
	float OriginalGroundFriction = 0.0f;

	UPROPERTY()
	bool bOrigianlUseControllerDesiredRotation = true;

	
};
