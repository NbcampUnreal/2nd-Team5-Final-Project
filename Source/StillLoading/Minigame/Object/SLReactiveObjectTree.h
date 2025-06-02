// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Minigame/Object/SLBaseReactiveObject.h"
#include "SLReactiveObjectTree.generated.h"

UCLASS()
class STILLLOADING_API ASLReactiveObjectTree : public ASLBaseReactiveObject
{
	GENERATED_BODY()
public:
	ASLReactiveObjectTree();

protected:
	virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType);

	virtual void Tick(float DeltaTime);
	
	virtual void BeginOverlapCollision(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
private:
	FORCEINLINE void Move(float DeltaTime);

private:
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1500;
};
