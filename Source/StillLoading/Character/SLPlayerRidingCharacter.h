// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLPlayerCharacterBase.h"
#include "SLPlayerRidingCharacter.generated.h"

struct FInputActionValue;
enum class EInputActionType : uint8;
class UDynamicIMCComponent;
class USLFlashComponent;

UCLASS()
class STILLLOADING_API ASLPlayerRidingCharacter : public ASLPlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASLPlayerRidingCharacter();
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) __override;
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnActionTriggeredCallback(const EInputActionType ActionType, const FInputActionValue InputValue);
	UFUNCTION()
	void OnActionStartedCallback(const EInputActionType ActionType);
	UFUNCTION()
	void OnActionCompletedCallback(const EInputActionType ActionType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDynamicIMCComponent> DynamicIMCComponent;

private:
	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, Category = "Flash")
	TObjectPtr<USLFlashComponent> FlashComponent;
};