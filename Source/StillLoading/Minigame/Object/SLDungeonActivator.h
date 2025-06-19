// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDungeonObjectInterface.h"
#include "SLDungeonActivator.generated.h"

UCLASS()
class STILLLOADING_API ASLDungeonActivator : public AActor, public ISLDungeonObjectInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASLDungeonActivator();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable)
	virtual void DeActivate() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
};
