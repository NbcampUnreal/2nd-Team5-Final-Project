// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLEditorGridVolume.generated.h"

UCLASS()
class STILLLOADING_API ASLEditorGridVolume : public AActor
{
	GENERATED_BODY()

public:
	ASLEditorGridVolume();
	
	UFUNCTION(BlueprintCallable, Category = "Editor")
	void ShowGrid();
	UFUNCTION(BlueprintCallable, Category = "Editor")
	void HideGrid();
	
protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override {return true;}
	
public:
	UPROPERTY(EditAnywhere, Category = "Grid")
	bool GridVisible = false;
	
	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 GridCount = 10;

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 GridHeight = 1080;
	
	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 GridWidth = 1920;

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 GroundHeight = 0;

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 CameraFitch = -40;
	
	UPROPERTY(EditAnywhere, Category = "Grid")
	FColor GridColor = FColor::Red;
	
	UPROPERTY(EditAnywhere, Category = "Grid")
	float LineThickness = 2.f;

};
