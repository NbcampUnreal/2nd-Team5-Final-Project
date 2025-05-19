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
	virtual void OnReacted(const ASLPlayerCharacter* InCharacter, ESLReactiveTriggerType TriggerType) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
private:
	// 회전 관련 변수
	UPROPERTY(EditAnywhere)
	TArray<FRotator> RotationStates;

	UPROPERTY(VisibleAnywhere)
	int32 CurrentRotationIndex = 0;

	UPROPERTY(EditAnywhere)
	int32 LerpSpeed = 20;


	TSoftObjectPtr<ASLMinigamePuzzleCond> PuzzleManager;

	FTimerHandle RotationHandle;
};
