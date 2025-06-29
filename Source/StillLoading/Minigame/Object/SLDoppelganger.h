// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDoppelganger.generated.h"

class UWidgetComponent;
class USceneCaptureComponent2D;
UCLASS()
class STILLLOADING_API ASLDoppelganger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLDoppelganger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UWidgetComponent> ProgressBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProgressSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentProgress;
};
