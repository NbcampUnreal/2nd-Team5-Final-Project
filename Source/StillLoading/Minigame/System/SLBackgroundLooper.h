// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLBackgroundLooper.generated.h"

UCLASS()
class STILLLOADING_API ASLBackgroundLooper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLBackgroundLooper();

	virtual void Tick(float DeltaTime) override;

	void CheckAndResetTile();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> Tiles;

	UPROPERTY(EditAnywhere, Category = "Loop Settings")
	float ScrollSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Loop Settings")
	float TileLength = 1000.0f; // 타일의 X 길이

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
};
