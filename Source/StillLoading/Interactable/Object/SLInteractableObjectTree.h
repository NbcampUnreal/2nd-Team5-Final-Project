// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SLInteractableObjectTree.generated.h"

UCLASS()
class STILLLOADING_API ASLInteractableObjectTree : public ASLInteractableObjectBase
{
	GENERATED_BODY()
public:
	ASLInteractableObjectTree();

	UFUNCTION()
	void SetStaticMesh(UStaticMesh* StaticMesh);
protected:
	UFUNCTION()
	void BeginOverlapCharacter(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	virtual void OnInteracted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType);
	
	virtual void Tick(float DeltaTime);
	
	virtual void BeginPlay() override;
private:
	FORCEINLINE void Move(float DeltaTime);

private:
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1500;
};
