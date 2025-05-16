// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChargingWidgetActor.generated.h"

class UWidgetComponent;

UCLASS()
class STILLLOADING_API AChargingWidgetActor : public AActor
{
	GENERATED_BODY()

public:
	AChargingWidgetActor();

	void SetTargetCharacter(ACharacter* Target);

	UPROPERTY(VisibleAnywhere)
	FVector Offset = FVector(0.f, 0.f, 30.f);

	UPROPERTY(VisibleAnywhere)
	FName SocketName = "head1Socket";
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* WidgetComponent;

	UPROPERTY()
	ACharacter* TargetCharacter;

public:
	FORCEINLINE UWidgetComponent* GetWidgetComponent() const { return WidgetComponent; }
};
