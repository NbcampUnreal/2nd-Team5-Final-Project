// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLReactiveObjectStatue.h"
#include "SLReactiveObjectLuminousStatue.generated.h"

class UPointLightComponent;

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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> PointLightComp = nullptr;

	float DeltaTime = 0;

	UPROPERTY(EditAnywhere)
	float Intensity = 50;

	UPROPERTY(EditAnywhere)
	float LightChangeSpeed = 1.0f;

	FTimerHandle LightControlHandler;
};
