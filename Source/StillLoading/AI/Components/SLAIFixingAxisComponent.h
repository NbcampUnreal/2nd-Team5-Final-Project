// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLAIFixingAxisComponent.generated.h"


class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STILLLOADING_API USLAIFixingAxisComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USLAIFixingAxisComponent();

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
protected:
    virtual void BeginPlay() override;

private:

    UPROPERTY()
    TObjectPtr<UCharacterMovementComponent> MovementComponent;

    UPROPERTY()
    TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent; 
    
};
