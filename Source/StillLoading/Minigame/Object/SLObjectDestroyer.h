// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLObjectDestroyer.generated.h"




UCLASS()

class STILLLOADING_API ASLObjectDestroyer : public AActor
{
	GENERATED_BODY()
	
public:	
	
	// Sets default values for this actor's properties
	ASLObjectDestroyer();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHitDoorEvent);

	UPROPERTY(BlueprintAssignable)
	FOnHitDoorEvent OnHitDoor;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void DestroySelf();
};
