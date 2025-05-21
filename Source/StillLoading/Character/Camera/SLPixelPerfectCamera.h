// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "SLPixelPerfectCamera.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API USLPixelPerfectCamera : public UCameraComponent
{
	GENERATED_BODY()

public:
	USLPixelPerfectCamera();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetPixelSize(const FVector2D NewPixelSize){PixelSize = NewPixelSize;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector2D GetPixelSize() const {return PixelSize;}
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Pixel Perfect Camera")
	void InitPixelSize();
	
	
private:
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;

	UPROPERTY(VisibleAnywhere)
	FVector2D PixelSize;

};
