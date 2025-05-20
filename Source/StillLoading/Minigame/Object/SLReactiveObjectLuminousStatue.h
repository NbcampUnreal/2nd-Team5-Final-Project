// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLReactiveObjectStatue.h"

#include "SLReactiveObjectLuminousStatue.generated.h"

class UPointLightComponent;
class ASLMinigamePuzzleCond;

UCLASS()
class STILLLOADING_API ASLReactiveObjectLuminousStatue : public ASLReactiveObjectStatue
{
	GENERATED_BODY()
public:
	ASLReactiveObjectLuminousStatue();
	UFUNCTION()
	void TurnOffLight();

protected:
	virtual void BeginPlay() override;
	virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType) override;
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
