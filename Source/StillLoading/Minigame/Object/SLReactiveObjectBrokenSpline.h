// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLReactiveObjectBreakable.h"
#include "SLReactiveObjectBrokenSpline.generated.h"

UCLASS()
class STILLLOADING_API ASLReactiveObjectBrokenSpline : public ASLReactiveObjectBreakable
{
    GENERATED_BODY()

public:
    ASLReactiveObjectBrokenSpline();
    
    virtual void OnReacted(const ASLPlayerCharacterBase* InCharacter, ESLReactiveTriggerType InTriggerType);

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnBroken();
};
