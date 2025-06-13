// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLInteractableObjectStatue.h"

#include "SLInteractableObjectLuminousStatue.generated.h"

class UPointLightComponent;
class ASLMinigamePuzzleCond;

UCLASS()
class STILLLOADING_API ASLInteractableObjectLuminousStatue : public ASLInteractableObjectStatue
{
	GENERATED_BODY()
public:
	ASLInteractableObjectLuminousStatue();
	UFUNCTION()
	void TurnOffLight();

protected:
	virtual void BeginPlay() override;
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType) override;
	UFUNCTION()
	void SetLightActive();
private:
	UFUNCTION()
	void ResetCondition();

	UPROPERTY(VisibleAnywhere)
	bool bLightOn;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> PointLightComp = nullptr;



	FTimerHandle LightControlHandler;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ASLMinigamePuzzleCond> PuzzleManager;
};
