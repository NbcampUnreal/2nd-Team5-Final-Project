// Fill out your copyright notice in the Description page of Project Settings.


#include "SLCameraHandlerComponent.h"

#include "Camera/CameraComponent.h"

USLCameraHandlerComponent::USLCameraHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void USLCameraHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void USLCameraHandlerComponent::AssignCamera(UCameraComponent* InThirdPersonCamera, UCameraComponent* InTopViewCamera)
{
	ThirdPersonCamera = InThirdPersonCamera;
	TopViewCamera = InTopViewCamera;

	ThirdPersonCamera->SetActive(false);
	TopViewCamera->SetActive(true);
}

