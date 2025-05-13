// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLReactiveObjectPortal.generated.h"

/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLReactiveObjectPortal : public ASLBaseReactiveObject
{
	GENERATED_BODY()
public:
	ASLReactiveObjectPortal();
protected:
	virtual void OnReacted(const ASLBaseCharacter*, ESLReactiveTriggerType TriggerType);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComp;
private:

	UPROPERTY(EditAnywhere)
	FName DestinationLevelName;

	UPROPERTY(EditAnywhere)
	int8 ObjectHp;

	UPROPERTY(EditAnywhere)
	int8 Strength;// Damage
	UPROPERTY(EditAnywhere)
	int8 Location;// 위치
	UPROPERTY(EditAnywhere)
	int8 Radius;// Radius
};
