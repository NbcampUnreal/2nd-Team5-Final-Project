// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SLInteractableObjectAnimation.generated.h"

class UAnimMontage;
class UArrowComponent;

UCLASS()
class STILLLOADING_API ASLInteractableObjectAnimation : public ASLInteractableObjectBase
{
	GENERATED_BODY()
	
public:
	ASLInteractableObjectAnimation();

protected:
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType) override;

	UFUNCTION()
	void ExecuteEnterAnimation(ASLPlayerCharacterBase* InCharacter);

	UFUNCTION()
	void ExecuteExitAnimation(ASLPlayerCharacterBase* InCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Hiding")
	void OnPlayerEnterHiding(ASLPlayerCharacterBase* InCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Hiding")
	void OnPlayerExitHiding(ASLPlayerCharacterBase* InCharacter);

	UFUNCTION()
	void OnEnterMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnExitMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> EnterTransformArrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimMontage> EnterMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimMontage> ExitMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsPlayerHiding;

private:
	UPROPERTY()
	TObjectPtr<ASLPlayerCharacterBase> CachedCharacter;
};