// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Character/CameraManagerComponent/SLCameraType.h"

#include "SLCameraManagerActor.generated.h"

UCLASS()
class STILLLOADING_API ASLCameraManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLCameraManagerActor();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* TopDownCamera;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CinematicCamera;

	void SetCameraMode(EGameCameraType Mode); // 내부 활성 전환

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void TargetFollow(float DeltaTime);

	EGameCameraType CurrentCameraMode;

};
