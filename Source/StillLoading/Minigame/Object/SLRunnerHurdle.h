// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/SLPlayerRunnerCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLRunnerHurdle.generated.h"

class UBoxComponent;

UCLASS()
class STILLLOADING_API ASLRunnerHurdle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLRunnerHurdle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	const EHurdleState GetHurdleState() { return HurdleState; }
	const ERunnerMontageSection GetRunnerMontageSection() { return RunnerMontageSection; }
	void SetDefaultHurdleState() { HurdleState = EHurdleState::None; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SceneComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> BoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHurdleState HurdleState = EHurdleState::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERunnerMontageSection RunnerMontageSection = ERunnerMontageSection::None;
};
