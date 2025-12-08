// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLInteractableHighlight.generated.h"

class UPointLightComponent;

UCLASS()
class STILLLOADING_API USLInteractableHighlight : public USceneComponent
{
	GENERATED_BODY()

public:
	USLInteractableHighlight();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void HighlightActivate();

	UFUNCTION(BlueprintCallable)
	void HighlightDeactivate();

	void SetInteracted(bool bInput);
	
	UPROPERTY(EditAnywhere, Category = "InteractableHighlight")
	TObjectPtr<UPointLightComponent> PointLight;
	
protected:
	virtual void BeginPlay() override;

private:
	void SetupLight();
	
	UPROPERTY(EditAnywhere, Category = "InteractableHighlight")
	float LightSpeed = 3.0f;

	UPROPERTY(EditAnywhere, Category = "InteractableHighlight")
	float LightIntensityMax = 60000.0f;

	float CurrentTime = 0.0f;

	bool bIsInteracted = false;
};
