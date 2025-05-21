// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLCameraHandlerComponent.generated.h"

class UCameraComponent;

UENUM(BlueprintType)
enum class ECameraType : uint8
{
	ECT_None,
	ECT_FirstPerson,
	ECT_ThirdPerson,
	ECT_PixelPerfect,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLCameraHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLCameraHandlerComponent();
	
	UFUNCTION(BlueprintCallable)
	void AssignCamera(UCameraComponent* InThirdPersonCamera, UCameraComponent* InTopViewCamera);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> MainCamera;
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	ECameraType CameraType;

	UPROPERTY()
	TObjectPtr<UCameraComponent> ThirdPersonCamera;
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> TopViewCamera;
};
