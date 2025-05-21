// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLReactiveObjectStatue.h"
#include "SLReactiveObjectRotationStatue.generated.h"

class ASLMinigamePuzzleCond;

UCLASS()
class STILLLOADING_API ASLReactiveObjectRotationStatue : public ASLReactiveObjectStatue
{
	GENERATED_BODY()

public:
	ASLReactiveObjectRotationStatue();
protected:
	virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType TriggerType) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
private:

	UFUNCTION()
	void SetRotate(FRotator TargetRotator);

	UFUNCTION()
	void ResetCondition();

	UFUNCTION()
	void RotateActor(float DeltaTime);

	bool bRotateTrigger = false;

	bool ResetTrigger = false;

	// 회전 관련 변수
	UPROPERTY(EditAnywhere)
	TArray<FRotator> RotationStates;

	UPROPERTY()
	FRotator InitialRotation;

	FRotator StartRotation;

	FRotator TargetRotation;

	UPROPERTY(VisibleAnywhere)
	int32 CurrentRotationIndex = 0;

	UPROPERTY(EditAnywhere)
	int32 LerpSpeed = 5.0f;

	UPROPERTY(EditAnywhere)
	float RotationDuration = 1.0f; //회전 시간

	UPROPERTY()
	float RotationElapsed = 0.0f;


	TSoftObjectPtr<ASLMinigamePuzzleCond> PuzzleManager;

	FTimerHandle RotationHandle;
};
