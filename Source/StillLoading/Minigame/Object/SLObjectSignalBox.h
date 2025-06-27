// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SLObjectSignalBox.generated.h"


UCLASS()
class STILLLOADING_API ASLObjectSignalBox : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASLObjectSignalBox();

	UPROPERTY()
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, Category = "SignalSetting")
	TArray<TObjectPtr<AActor>> TargetActors;

	UPROPERTY(EditAnywhere, Category = "SignalSetting")
	bool bEndOverlapDeActivate;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DeactivateTrapFloor();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> BeginOverlapSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> EndOverlapSound;
private:
	UPROPERTY(EditAnywhere)
	FName EnterFunctionName = "Activate";

	UPROPERTY(EditAnywhere)
	FName ExitFunctionName = "DeActivate";
	
	UPROPERTY(EditAnywhere, Category = "ActivateSetting")
	float VisibleTime = 5.0f;

	UPROPERTY()
	FTimerHandle DeactivateHandler;

	UPROPERTY()
	bool bIsActive = false;
};
