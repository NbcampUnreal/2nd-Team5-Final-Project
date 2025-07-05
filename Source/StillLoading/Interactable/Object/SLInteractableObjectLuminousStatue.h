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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType) override;
	UFUNCTION()
	void SetLightActive();
private:
	UFUNCTION()
	void ResetCondition();
	
	// 타이머 콜백 함수들
	void UpdateLightIntensity();
	void StartLightTransition(float InStartIntensity, float InEndIntensity, bool bShouldUpdatePuzzle = false);
	void StopLightTransition();

	UPROPERTY(VisibleAnywhere)
	bool bLightOn;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> PointLightComp = nullptr;

	FTimerHandle LightControlHandler;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ASLMinigamePuzzleCond> PuzzleManager;
	
	// 타이머 관련 변수들
	float TransitionElapsedTime;
	float TransitionTargetTime;
	float TransitionStartIntensity;
	float TransitionEndIntensity;
	bool bShouldUpdatePuzzleOnComplete;
	
	static constexpr float TRANSITION_INTERVAL = 0.05f;
	static constexpr float TRANSITION_DURATION = 1.0f;
	static constexpr float MAX_INTENSITY = 20.0f;
	static constexpr float MIN_INTENSITY = 0.0f;
};