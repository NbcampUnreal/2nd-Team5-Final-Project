// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "GeometryCollection\GeometryCollectionComponent.h"

#include "SLInteractableObjectPortal.generated.h"

class ASLObjectDestroyer;
class UArrowComponent;
/**
 * 
 */
UCLASS()
class STILLLOADING_API ASLInteractableObjectPortal : public ASLInteractableObjectBase
{
	GENERATED_BODY()
public:
	ASLInteractableObjectPortal();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASLObjectDestroyer> DestroyerActor;

	

protected:
	virtual void BeginPlay() override;
	virtual void OnInteracted(const ASLPlayerCharacterBase*, ESLReactiveTriggerType TriggerType);

	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComp;
private:
	UFUNCTION()
	void BrokenDoor();

	UFUNCTION()
	void OnBroken();

	UPROPERTY(EditAnywhere)
	FName DestinationLevelName;

	UPROPERTY(EditAnywhere)
	int8 ObjectHp;

	UPROPERTY(EditAnywhere)
	int8 Strength;// Damage
	UPROPERTY(EditAnywhere)
	int8 Location;// 위치
	UPROPERTY(EditAnywhere)
	int32 Radius;// Radius

	UPROPERTY()
	FVector SpawnLocation;

	UPROPERTY()
	FRotator SpawnRotation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> CollisionComp;
};
